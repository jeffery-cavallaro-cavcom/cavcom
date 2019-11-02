#ifndef CAVCOM_UTILITY_LIBUTIL_LOOKUP_H_
#define CAVCOM_UTILITY_LIBUTIL_LOOKUP_H_

#include <map>
#include <sstream>
#include <stdexcept>

namespace cavcom {
  namespace utility {
    // A lookup table that enforces uniqueness.  The K and V template arguments specify the key/value types.  The
    // key type must be automatically string-convertible.
    template<typename K, typename V> class Lookup {
     public:
      // Lookup not found and duplicate exceptions.
      class LookupError : public std::runtime_error {
       public:
        // Creates a new lookup error instance.  The what message is constructed as follows:
        //
        //    <reason> <name>: <key>
        //
        LookupError(const std::string &reason, const std::string &name, const K &key)
          : std::runtime_error(reason), key_(key) {
          std::ostringstream formatter;
          formatter << std::runtime_error::what();
          formatter << " ";
          formatter << name;
          formatter << ": ";
          formatter << key_;
          msg_ = formatter.str();
        }
        virtual const char *what(void) const noexcept { return msg_.c_str(); }
        const K &key(void) const { return key_; }
       private:
        const K key_;
        std::string msg_;
      };

      class NotFoundLookupError : public LookupError {
       public:
        NotFoundLookupError(const std::string &name, const K &key) : LookupError("Unknown", name, key) {}
      };

      class DuplicateLookupError : public LookupError {
       public:
        DuplicateLookupError(const std::string &name, const K &key) : LookupError("Duplicate", name, key) {}
      };

     public:
      // Creates a new, empty lookup table.  The name argument is a description name for the key type that appears
      // in exception messages.
      explicit Lookup(const std::string &name = "key") : name_(name) {}

      // Returns the number of key/value pairs in the lookup table.
      using LookupTable = std::map<K, V>;
      typename LookupTable::size_type size(void) const { return lookup_.size(); }

      // Returns true and the corresponding value if the specified key is known.  Otherwise, returns false or
      // throws a not found error if exceptions are enabled by the errors argument.
      bool find(const K &key, V *value, bool errors = false) const {
        const auto &i = lookup_.find(key);
        if (i == lookup_.end()) {
          if (errors) throw NotFoundLookupError(name_, key);
          return false;
        }
        *value = i->second;
        return true;
      }

      // Adds the specified key/value pair to the lookup table.  Returns true if the key was not already in use.
      // Otherwise, returns false or throws a duplicate error if exceptions are enabled by the errors argument.
      bool add(const K &key, const V &value, bool errors = false) {
        bool success = lookup_.emplace(key, value).second;
        if ((!success) && errors) throw DuplicateLookupError(name_, key);
        return success;
      }

      // Discards the key/value pair associated with the specified key from the lookup table.  Does nothing if the
      // key is not known.
      void remove(const K &key) { lookup_.erase(key); }

      // Discards all entries in the lookup table.
      void clear(void) { lookup_.clear(); }

     private:
      const std::string name_;
      LookupTable lookup_;
    };

  }  // namespace utility
}  // namespace cavcom

#endif  // CAVCOM_UTILITY_LIBUTIL_LOOKUP_H_
