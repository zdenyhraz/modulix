
int main(int argc, char** argv)
try
{
  LOG_DEBUG("Hello world!");
  return EXIT_SUCCESS;
}
catch (const std::exception& e)
{
  LOG_EXCEPTION(e);
  return EXIT_FAILURE;
}
