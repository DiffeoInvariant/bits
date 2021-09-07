#ifndef BITS_LOGGER_H
#define BITS_LOGGER_H
#include <string>
#include <iostream>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <vector>
#include <mutex>
#include <chrono>
#include <tuple>
#include <ctime>
#include <optional>

namespace bits
{

  enum class log_level : int {
    NOTSET=0,
    DEBUG=10,
    INFO=20,
    WARNING=30,
    ERROR=40,
    CRITICAL=50
  };

  template<
    class CharType,
    class Traits = std::char_traits<CharType>,
    class ClockType = std::chrono::system_clock,
    class DurationType = std::chrono::system_clock::duration,
    class StringAllocator = std::allocator<CharType>,
    class Allocator =
      std::allocator<
        std::tuple<
	  std::basic_string<CharType, Traits, StringAllocator>,
	  log_level,
	  std::chrono::time_point<ClockType>
	  >
      >
	   >
  class basic_in_memory_storage
  {
  public:

    using clock_type = ClockType;
    using string_type = std::basic_string<CharType, Traits, StringAllocator>;
    using time_point_type = std::chrono::time_point<ClockType>;
    using entry_type = std::tuple<string_type, log_level, time_point_type>;
    using buffer_type = std::vector<entry_type>;
    using traits_type = Traits;
    using value_type = CharType;
    using allocator_type = Allocator;
    using size_type = typename std::allocator_traits<Allocator>::size_type;
    using difference_type = typename std::allocator_traits<Allocator>::difference_type;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator = typename buffer_type::iterator;
    using const_iterator = typename buffer_type::const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    basic_in_memory_storage() = default;
    
    basic_in_memory_storage(size_type buf_size)
      : m_buffer{buf_size}
    {}

    basic_in_memory_storage(const basic_in_memory_storage<CharType, Traits,
			    ClockType, DurationType, StringAllocator, Allocator>&) = default;
    basic_in_memory_storage(basic_in_memory_storage<CharType, Traits,
			    ClockType, DurationType, StringAllocator, Allocator> && ) = default;


    basic_in_memory_storage& write(const string_type& string, log_level level,
				   const time_point_type& time)
    {
      m_buffer.push_back({string, level, time});
      return *this;
    }

    basic_in_memory_storage& write(const value_type *string, log_level level,
				   const time_point_type& time)
    {
      m_buffer.push_back({string_type(string), level, time});
      return *this;
    }

    string_type formatted_entry(const string_type& message,
				log_level level,
				const time_point_type& time)
    {
      return formatted(message, level, time);
    }

    string_type formatted_entry(const value_type *message,
				log_level level,
				const time_point_type& time)
    {
      return formatted(string_type(message), level, time);
    }

    string_type read(log_level minlevel = log_level::NOTSET) const noexcept
    {
      return read(0, minlevel);
    }

    /* read from character number `start` to the end */
    string_type read(size_type start, log_level minlevel) const
    {
      return read(start, string_type::npos, minlevel);
    }

    string_type read(size_type start, size_type nentry,
		     log_level minlevel) const
    {
      string_type result;
      if (start >= size()) {
	throw std::out_of_range(std::string{"Error, in_memory_buffer has "} + std::to_string(size()) + " but you requested entries but entries starting at number " + std::to_string(start));
      }
      size_type i=start, N=std::min(size(), start+nentry);
      for(; i<N-1; ++i) {
	if (std::get<1>(m_buffer[i]) >= minlevel) {
	  result += formatted(std::get<0>(m_buffer[i]), std::get<1>(m_buffer[i]), std::get<2>(m_buffer[i]));
	  result += newline();
	}
      }
      if (std::get<1>(m_buffer[N-1]) >= minlevel) {
	result += formatted(std::get<0>(m_buffer[N-1]), std::get<1>(m_buffer[N-1]), std::get<2>(m_buffer[N-1]));
      }
      return result;
    }

    constexpr string_type repr() const noexcept
    {
      if constexpr (std::is_same<value_type, char>::value) {
	return "in_memory_storage{" + read(log_level::NOTSET) + "}";
      } else if constexpr (std::is_same<value_type, wchar_t>::value) {
	return L"wide_in_memory_storage{" + read(log_level::NOTSET) + L"}";
      } else if constexpr (std::is_same<value_type, char8_t>::value) {
	return u8"utf8_in_memory_storage{" + read(log_level::NOTSET) + u8"}";
      } else if constexpr (std::is_same<value_type, char16_t>::value) {
	return u"utf16_in_memory_storage{" + read(log_level::NOTSET) + u"}";
      } else if constexpr (std::is_same<value_type, char32_t>::value) {
	return U"utf32_in_memory_storage{" + read(log_level::NOTSET) + U"}";
      } 
    }

    constexpr iterator begin() noexcept
    {
      return m_buffer.begin();
    }
    
    constexpr const_iterator begin() const noexcept
    {
      return m_buffer.begin();
    }
    
    constexpr const_iterator cbegin() const noexcept
    {
      return m_buffer.cbegin();
    }

    constexpr iterator end() noexcept
    {
      return m_buffer.end();
    }

    constexpr const_iterator end() const noexcept
    {
      return m_buffer.end();
    }

    constexpr const_iterator cend() const noexcept
    {
      return m_buffer.cend();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
      return m_buffer.rbegin();
    }
    
    constexpr const_reverse_iterator rbegin() const noexcept
    {
      return m_buffer.rbegin();
    }
    
    constexpr const_reverse_iterator crbegin() const noexcept
    {
      return m_buffer.crbegin();
    }

    constexpr reverse_iterator rend() noexcept
    {
      return m_buffer.rend();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
      return m_buffer.crend();
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
      return m_buffer.empty();
    }

    constexpr size_type size() const noexcept
    {
      return m_buffer.size();
    }

    constexpr size_type num_entries() const noexcept
    {
      return size();
    }

    /* returns size in bytes of the whole buffer */
    constexpr size_type buffer_size() const noexcept
    {
      /* should we check for overflow? not doing that right now */
      size_type size = 0;
      for (const auto& entry : m_buffer) {
	size += sizeof(value_type) * entry.size();
      }
      return size;
    }

    constexpr void reserve(size_type new_capacity)
    {
      m_buffer.reserve(new_capacity);
    }

    constexpr size_type capacity() const noexcept
    {
      return m_buffer.capacity();
    }

    static constexpr string_type new_line() noexcept
    {
      return newline();
    }

    constexpr void clear() const noexcept
    {
      m_buffer.clear();
    }

    

  private:
    std::vector<std::tuple<
		  string_type,
		  log_level,
		  time_point_type
		  >
		> m_buffer;

    constexpr string_type level_name(log_level level) const noexcept
    {
      if constexpr (std::is_same<value_type, char>::value) {
	switch (level) {
	case log_level::NOTSET:
	  return "NOTSET";
	case log_level::DEBUG:
	  return "DEBUG";
	case log_level::INFO:
	  return "INFO";
	case log_level::WARNING:
	  return "WARNING";
	case log_level::ERROR:
	  return "ERROR";
	case log_level::CRITICAL:
	  return "CRITICAL";
	}
      } else if constexpr (std::is_same<value_type, wchar_t>::value) {	
	switch (level) {
	case log_level::NOTSET:
	  return L"NOTSET";
	case log_level::DEBUG:
	  return L"DEBUG";
	case log_level::INFO:
	  return L"INFO";
	case log_level::WARNING:
	  return L"WARNING";
	case log_level::ERROR:
	  return L"ERROR";
	case log_level::CRITICAL:
	  return L"CRITICAL";
	}
      } else if constexpr (std::is_same<value_type, char8_t>::value) {
	switch (level) {
	case log_level::NOTSET:
	  return u8"NOTSET";
	case log_level::DEBUG:
	  return u8"DEBUG";
	case log_level::INFO:
	  return u8"INFO";
	case log_level::WARNING:
	  return u8"WARNING";
	case log_level::ERROR:
	  return u8"ERROR";
	case log_level::CRITICAL:
	  return u8"CRITICAL";
	}
      } else if constexpr (std::is_same<value_type, char16_t>::value) {
	switch (level) {
	case log_level::NOTSET:
	  return u"NOTSET";
	case log_level::DEBUG:
	  return u"DEBUG";
	case log_level::INFO:
	  return u"INFO";
	case log_level::WARNING:
	  return u"WARNING";
	case log_level::ERROR:
	  return u"ERROR";
	case log_level::CRITICAL:
	  return u"CRITICAL";
	}
      } else if constexpr (std::is_same<value_type, char32_t>::value) {
	switch (level) {
	case log_level::NOTSET:
	  return U"NOTSET";
	case log_level::DEBUG:
	  return U"DEBUG";
	case log_level::INFO:
	  return U"INFO";
	case log_level::WARNING:
	  return U"WARNING";
	case log_level::ERROR:
	  return U"ERROR";
	case log_level::CRITICAL:
	  return U"CRITICAL";
	}
      } 
    }

    static constexpr string_type newline() noexcept
    {
      if constexpr (std::is_same<value_type, char>::value) {
	return "\n";
      } else if constexpr (std::is_same<value_type, wchar_t>::value) {
	return L"\n";
      } else if constexpr (std::is_same<value_type, char8_t>::value) {
	return u8"\n";
      } else if constexpr (std::is_same<value_type, char16_t>::value) {
	return u"\n";
      } else if constexpr (std::is_same<value_type, char32_t>::value) {
	return U"\n";
      }
    }

    virtual string_type formatted(const string_type& val, log_level level, const time_point_type& time) const 
    {
      char tbuf[100]; /* assume a timestamp is < 100 characters */
      auto now = ClockType::to_time_t(time);
      std::tm *now_tm = std::localtime(&now);
      std::string ctime;
      if (auto len = strftime(tbuf, sizeof(tbuf), "%A %c", now_tm)) {
	ctime = std::string{tbuf, len};
      } else {
	throw std::runtime_error("strftime failed");
      }
      
      if constexpr (std::is_same<value_type, char>::value) {
	return "(" + val + "," + level_name(level) + "," + ctime + ")";
      } else if constexpr (std::is_same<value_type, wchar_t>::value) {
	return L"(" + val + L"," + level_name(level) + L","
	  + std::wstring(ctime.begin(), ctime.end()) + L")";
      } else if constexpr (std::is_same<value_type, char8_t>::value) {
	return u8"(" + val + u8"," + level_name(level) + u8"," +
	  std::u8string(ctime.begin(), ctime.end()) + u8")";
      } else if constexpr (std::is_same<value_type, char16_t>::value) {
	return u"(" + val + u"," + level_name(level) +
	  u"," + std::u16string(ctime.begin(), ctime.end()) + u")";
      } else if constexpr (std::is_same<value_type, char32_t>::value) {
	return U"(" + val + U"," + level_name(level) + U"," +
	  std::u32string(ctime.begin(), ctime.end()) + U")";
      } 
    }
    
  };

  using in_memory_storage = basic_in_memory_storage<char>;
  using wide_in_memory_storage = basic_in_memory_storage<wchar_t>;
  using utf8_in_memory_storage = basic_in_memory_storage<char8_t>;
  using utf16_in_memory_storage = basic_in_memory_storage<char16_t>;
  using utf32_in_memory_storage = basic_in_memory_storage<char32_t>;
  
  template<class CharType,
	   class ClockType = std::chrono::system_clock,
	   class Storage = basic_in_memory_storage<CharType,
						   std::char_traits<CharType>,
						   ClockType,
						   std::chrono::duration<ClockType>
						   >,
	   
	   class OutputStream = std::basic_ostream<CharType>
	   >
  class basic_logger
  {
    log_level m_level = log_level::NOTSET;
    Storage m_backing;
    bool         m_has_ostream, m_preserve_all;
    OutputStream &m_os;

  public:

    using entry_type = typename Storage::entry_type;
    using backing_type = Storage;
    using clock_type = ClockType;
    using size_type = typename Storage::size_type;
    using char_type = CharType;
    using string_type = typename Storage::string_type;
    using iterator = typename Storage::iterator;
    using const_iterator = typename Storage::const_iterator;
    using reverse_iterator = typename Storage::reverse_iterator;
    using const_reverse_iterator = typename Storage::const_reverse_iterator;
    /* the user should never need to set this template parameter themselves
     (or even really know that it exists). It exists solely so that the 
    basic_logger type itself is well-formed (since at most one of these 
					     can be correct for any given
					     char_type)*/
    template<typename T = char_type>
    basic_logger(typename std::enable_if_t<std::is_same_v<T, char>,
		 OutputStream&> os = std::clog,
		 log_level llevel = log_level::NOTSET)
      : m_os{os},
	m_level{llevel},
	m_has_ostream{true}
    {}

    template<typename T = char_type>
    basic_logger(typename std::enable_if_t<std::is_same_v<T, wchar_t>,
		 OutputStream&> os = std::wclog,
		 log_level llevel = log_level::NOTSET)
      : m_os{os},
	m_level{llevel},
	m_has_ostream{true}
    {}

    template<typename T>
    basic_logger(std::optional<OutputStream> os = std::nullopt, log_level newlevel = log_level::NOTSET)
    {
      if (os) {
	m_os = *os;
      }
      m_level = newlevel;
    }

    basic_logger& set_level(log_level newlevel) noexcept
    {
      m_level = newlevel;
      return *this;
    }

    log_level level() const noexcept
    {
      return m_level;
    }

    /* if true, will save every entry for later inspection even if less than
       the minimum log level */
    basic_logger& set_persist_all(bool preserve = true)
    {
      m_preserve_all = preserve;
      return *this;
    }

    
    basic_logger& log(string_type message, log_level level, bool display=true)
    {
      if (level < m_level and not m_preserve_all) {
	return *this;
      }
      auto time = clock_type::now();
      m_backing.write(message, level, time);
      if (display and level >= m_level) {
	m_os << m_backing.formatted_entry(message, level, time)
	     << m_backing.new_line();
      }
      return *this;
    }

    string_type format_entry(const entry_type& it) {
      return m_backing.formatted_entry(std::get<0>(it),
				       std::get<1>(it),
				       std::get<2>(it));
    }

    /* iterators do not respect the minimum log level */

    iterator begin() noexcept
    {
      return m_backing.begin();
    }
    
    const_iterator begin() const noexcept
    {
      return m_backing.begin();
    }
    
    const_iterator cbegin() const noexcept
    {
      return m_backing.cbegin();
    }

    iterator end() noexcept
    {
      return m_backing.end();
    }

    const_iterator end() const noexcept
    {
      return m_backing.end();
    }

    const_iterator cend() const noexcept
    {
      return m_backing.cend();
    }
    
    reverse_iterator rbegin() noexcept
    {
      return m_backing.rbegin();
    }
    
    const_reverse_iterator rbegin() const noexcept
    {
      return m_backing.rbegin();
    }
    
    const_reverse_iterator crbegin() const noexcept
    {
      return m_backing.crbegin();
    }

    reverse_iterator rend() noexcept
    {
      return m_backing.rend();
    }

    const_reverse_iterator crend() const noexcept
    {
      return m_backing.crend();
    }

    

  };

  using logger = basic_logger<char>;
  using wlogger = basic_logger<wchar_t>;
  using u8logger = basic_logger<char8_t>;
  using u16logger = basic_logger<char16_t>;
  using u32logger = basic_logger<char32_t>;

} /* namespace bits */
#endif /* BITS_LOGGER_H */
