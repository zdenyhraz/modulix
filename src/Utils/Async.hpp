#pragma once

template <typename T>
inline void LaunchAsync(T&& fun)
{
  std::thread(
      [fun]()
      {
        try
        {
          fun();
        }
        catch (const std::exception& e)
        {
          LOG_EXCEPTION(e);
        }
      })
      .detach();
}

template <typename T>
inline void Launch(T&& fun)
{
  try
  {
    fun();
  }
  catch (const std::exception& e)
  {
    LOG_EXCEPTION(e);
  }
}
