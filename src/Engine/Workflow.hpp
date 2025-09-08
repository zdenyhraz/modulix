#pragma once
#include "Module.hpp"
#include "Modules/Start.hpp"

class Workflow {
  std::string name = "Default workflow";
  std::vector<std::unique_ptr<Module>> microservices;
  std::vector<Module::Connection> connections;

  void FetchInputParameters(Module &microservice) {
    auto relevantConnections = std::views::filter(
        microservice.GetInputConnections(), [](const auto &conn) {
          return conn.inputParameter->value and
                 not conn.inputParameter->value->has_value() and
                 conn.inputParameter !=
                     &conn.inputModule->GetFlowInputParameter();
        });

    if (std::ranges::distance(relevantConnections) == 0)
      return;

    for (auto &connection : relevantConnections)
      ExecuteModule(*connection.outputModule);
  }

  void Process(Module &microservice) { microservice.Process(); }

  void PropagateFlow(Module &microservice) {
    auto relevantConnections = std::views::filter(
        microservice.GetOutputConnections(), [](const auto &conn) {
          return conn.outputParameter ==
                 &conn.outputModule->GetFlowOutputParameter();
        });

    if (std::ranges::distance(relevantConnections) == 0)
      return;

    for (auto &connection : relevantConnections)
      ExecuteModule(*connection.inputModule);
  }

  void ExecuteModule(Module &microservice) {
    FetchInputParameters(microservice); // recusively fetch all input parameters
    Process(microservice);              // do the processing
    PropagateFlow(
        microservice); // notify connected microservices to start processing
  }

public:
  Workflow() { microservices.push_back(std::make_unique<Start>()); }

  Workflow(const std::filesystem::path &path) { Load(path); }

  void Load(const std::filesystem::path &path) {
    // TODO: load microservices from file (with parameters!)
    // TODO: load connections from file
    // TODO: connect the parameters
    // TODO: set workflow name according to file name
    // TODO: set node locations from file
  }

  void Save(const std::filesystem::path &path) const {
    // TODO: serialize microservices to json (with parameters!)
    // TODO: serialize connections to json
  }

  const std::string &GetName() { return name; }
  const std::vector<std::unique_ptr<Module>> &GetModules() {
    return microservices;
  }
  Module &GetStart() { return *microservices.front(); }
  const std::vector<Module::Connection> &GetConnections() {
    return connections;
  }

  template <typename T> Module &AddModule() {
    microservices.push_back(std::make_unique<T>());
    return *microservices.back();
  }

  void Run() try {
    LOG_SCOPE("Workflow Run");
    LOG_DEBUG("Running workflow '{}'", GetName());

    if (microservices.empty())
      throw std::runtime_error(fmt::format(
          "Workflow '{}' is missing a start microservice", GetName()));

    if (microservices.size() == 1)
      return LOG_WARNING("Workflow '{}' does not contain any microservices",
                         GetName());

    ExecuteModule(*microservices.front());
    Reset();
  } catch (const std::exception &e) {
    LOG_ERROR("Workflow '{}' error: {}", GetName(), e.what());
  }

  void Load() {
    LOG_SCOPE("Workflow Load");
    LOG_DEBUG("Loading workflow '{}'", GetName());
    for (auto &microservice : microservices)
      microservice->Load();
  }

  void Unload() {
    LOG_DEBUG("Unloading workflow '{}'", GetName());
    for (auto &microservice : microservices)
      microservice->Unload();
  }

  void Reset() {
    for (auto &microservice : microservices)
      microservice->Reset();
  }

  void Connect(Module::Connection &&connection) {
    if (not connection.outputModule or not connection.inputModule or
        not connection.outputParameter or not connection.inputParameter)
      return LOG_WARNING("Ignoring invalid connection");

    if (std::ranges::any_of(connections, [&connection](const auto &conn) {
          return conn == connection;
        }))
      return LOG_WARNING("Ignoring duplicate connection {}",
                         connection.GetString());

    if (connection.outputParameter->type != connection.inputParameter->type)
      return LOG_WARNING("Connection {} type mismatch: {} != {}",
                         connection.GetString(),
                         connection.inputParameter->type.name(),
                         connection.outputParameter->type.name());

    if (connection.inputParameter->type != typeid(Module::FlowParameter))
      if (std::ranges::any_of(connections, [&connection](const auto &conn) {
            return conn.inputParameter == connection.inputParameter;
          }))
        return LOG_WARNING("Connection to {}:{} already exists",
                           connection.inputModule->GetName(),
                           connection.inputParameter->GetName());

    if (connection.outputModule == connection.inputModule)
      return LOG_WARNING(
          "Connection {}: cannot connect microservice to itself ",
          connection.GetString());

    LOG_TRACE("Connected {}", connection.GetString());

    connection.inputParameter->value = &connection.outputParameter->value;
    connection.outputModule->AddOutputConnection(connection);
    connection.inputModule->AddInputConnection(connection);
    connections.push_back(std::move(connection));
  }

  void Connect(Module &outputModule, Module &inputModule,
               const std::string &outputParameterName,
               const std::string &inputParameterName) {
    auto &outputParameter =
        outputModule.GetOutputParameter(outputParameterName);
    auto &inputParameter = inputModule.GetInputParameter(inputParameterName);
    Module::Connection connection(&outputModule, &inputModule, &outputParameter,
                                  &inputParameter);
    Connect(std::move(connection));
  }

  void Connect(Module &outputModule, Module &inputModule,
               const std::string &parameterName) {
    auto &outputParameter = outputModule.GetOutputParameter(parameterName);
    auto &inputParameter = inputModule.GetInputParameter(parameterName);
    Module::Connection connection(&outputModule, &inputModule, &outputParameter,
                                  &inputParameter);
    Connect(std::move(connection));
  }

  void Connect(Module &outputModule, Module &inputModule) {
    Module::Connection connection(&outputModule, &inputModule,
                                  &outputModule.GetFlowOutputParameter(),
                                  &inputModule.GetFlowInputParameter());
    Connect(std::move(connection));
  }

  void Disconnect(uintptr_t connectionId) {
    auto conn =
        std::ranges::find_if(connections, [connectionId](const auto &conn) {
          return conn.GetId() == connectionId;
        });
    if (conn == connections.end())
      return LOG_WARNING(
          "Ignoring disconnect of connection {}: connection not found",
          connectionId);

    const auto connection = *conn;
    connections.erase(std::ranges::remove(connections, connection).begin(),
                      connections.end());
    connection.inputParameter->value = nullptr;
    connection.outputModule->RemoveOutputConnection(connection);
    connection.inputModule->RemoveInputConnection(connection);
    return LOG_TRACE("Disconnected connection {}", connection.GetString());
  }
};
