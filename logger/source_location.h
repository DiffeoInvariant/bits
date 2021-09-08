#ifndef BITS_SOURCE_LOCATION_H
#define BITS_SOURCE_LOCATION_H
#include <cstdint>
namespace bits
{
  class source_location
  {
    const char *m_file;
    uint_least32_t m_line, m_col;
    const char *m_function;
  public:
    
    static constexpr source_location current(const char *file=__builtin_FILE(),
				     const char *function=__builtin_FUNCTION(),
				     int line = __builtin_LINE(),
				     int col = 0) noexcept
    {
      source_location loc;
      loc.m_file = file;
      loc.m_function = function;
      loc.m_line = line;
      loc.m_col = col;
      return loc;
    }

    constexpr source_location() noexcept
    : m_file("unknown"), m_function("unknown"),
      m_line(0), m_col(0)
    {};

    constexpr uint_least32_t line() const noexcept { return m_line; }
    constexpr uint_least32_t column() const noexcept { return m_col; }
    constexpr const char * file_name() const noexcept { return m_file; }
    constexpr const char * function_name() const noexcept { return m_function; }
  };

}
#endif /* BITS_SOURCE_LOCATION_H */
