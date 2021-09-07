#ifndef BITS_LOGGER_H
#define BITS_LOGGER_H
#include <string>
#include <ostream>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <stdexcept>
#include <utility>
#include <algorithm>

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
    
  
  template<class CharType,
	   class Traits = std::char_traits<CharType>,
	   class Allocator = std::allocator<CharType>
	   >
  class basic_in_memory_storage
  {
    std::vector<std::pair<
		  std::basic_string<CharType, Traits, Allocator>,
		  log_level
		  >> m_buffer;

    
    
  public:

    using string_type = std::basic_string<CharType, Traits, Allocator>;
    using buffer_type = std::vector<std::pair<string_type, log_level>>;
    using traits_type = Traits;
    using value_type = CharType;
    using allocator_type = Allocator;
    using size_type = typename std::allocator_traits<Allocator>::size_type;
    using difference_type = typename std::allocator_traits<Allocator>::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
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

    basic_in_memory_storage(const basic_in_memory_storage<CharType, Traits, Allocator>&) = default;
    basic_in_memory_storage(basic_in_memory_storage<CharType, Traits, Allocator> && ) = default;


    constexpr basic_in_memory_storage& write(const string_type& string, log_level level)
    {
      m_buffer.push_back({string, level});
      return *this;
    }

    constexpr basic_in_memory_storage& write(const value_type *string, log_level level)
    {
      m_buffer.push_back({string_type(string), level});
      return *this;
    }

    constexpr string_type read(log_level minlevel = log_level::NOTSET) const noexcept
    {
      return read(0, minlevel);
    }

    /* read from character number `start` to the end */
    constexpr string_type read(size_type start, log_level minlevel) const
    {
      return read(start, string_type::npos, minlevel);
    }

    constexpr string_type read(size_type start, size_type nentry,
			       log_level minlevel) const
    {
      string_type result;
      if (start >= size()) {
	throw std::out_of_range(std::string{"Error, in_memory_buffer has "} + std::to_string(size()) + " but you requested entries but entries starting at number " + std::to_string(start));
      }
      size_type i=start, N=std::min(size(), start+nentry);
      for(; i<N-1; ++i) {
	if (m_buffer[i].second >= minlevel) {
	  result += formatted(m_buffer[i].first, m_buffer[i].second);
	  result += newline();
	}
      }
      if (m_buffer[N-1].second >= minlevel) {
	result += formatted(m_buffer[N-1].first, m_buffer[N-1].second);
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

    constexpr reverse_iterator crend() noexcept
    {
      return m_buffer.crend();
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

  private:

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

    constexpr string_type newline() const noexcept
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

    constexpr string_type formatted(const string_type& val, log_level level) const noexcept
    {
      if constexpr (std::is_same<value_type, char>::value) {
	return "(" + val + ", level=" + level_name(level) + ")";
      } else if constexpr (std::is_same<value_type, wchar_t>::value) {
	return L"(" + val + L", level=" + level_name(level) + L")";
      } else if constexpr (std::is_same<value_type, char8_t>::value) {
	return u8"(" + val + u8", level=" + level_name(level) + u8")";
      } else if constexpr (std::is_same<value_type, char16_t>::value) {
	return u"(" + val + u", level=" + level_name(level) + u")";
      } else if constexpr (std::is_same<value_type, char32_t>::value) {
	return U"(" + val + U", level=" + level_name(level) + U")";
      } 
    }
    
  };

  using in_memory_storage = basic_in_memory_storage<char>;
  using wide_in_memory_storage = basic_in_memory_storage<wchar_t>;
  using utf8_in_memory_storage = basic_in_memory_storage<char8_t>;
  using utf16_in_memory_storage = basic_in_memory_storage<char16_t>;
  using utf32_in_memory_storage = basic_in_memory_storage<char32_t>;
  
  template<class CharType,
	   class Storage = basic_in_memory_storage<CharType>
	   >
  class basic_logger
  {
    log_level level = log_level::NOTSET;
    Storage backing;
    

  };

} /* namespace bits */
#endif /* BITS_LOGGER_H */
