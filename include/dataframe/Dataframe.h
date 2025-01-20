#pragma once
#include <dataframe/Serie.h>
#include <map>
#include <string>

namespace df {

// -----------------------------------------------------

// Interface for the erasure type
class SerieBase {
  public:
    virtual ~SerieBase() = default;
    virtual uint32_t count() const = 0;
    virtual uint32_t itemSize() const = 0;
    virtual uint dimension() const = 0;
    virtual void print(uint32_t precision = 4) const = 0;
    virtual bool isValid() const = 0;
    virtual std::string type_name() const = 0;
    virtual std::unique_ptr<SerieBase> clone() const = 0;
};

// -----------------------------------------------------

// Wrapper
template <typename T> class SerieWrapper : public SerieBase {
  public:
    SerieWrapper(const GenSerie<T> &serie);

    uint32_t count() const override;
    uint32_t itemSize() const override;
    uint dimension() const override;
    bool isValid() const override;

    void print(uint32_t precision = 4) const override;
    std::string type_name() const override;
    std::unique_ptr<SerieBase> clone() const override;
    const GenSerie<T> &get() const;
    GenSerie<T> &get();

  private:
    GenSerie<T> serie_;
};

// -----------------------------------------------------

class DataFrame {
  public:
    template <typename T> void add(const std::string &, const GenSerie<T> &);
    template <typename T> const GenSerie<T> &get(const std::string &) const;
    template <typename T> GenSerie<T> &get(const std::string &);

    bool has(const std::string &) const;
    void remove(const std::string &);
    std::vector<std::string> names() const;
    std::string get_type(const std::string &) const;
    size_t size() const;
    void print(uint32_t precision = 4) const;

  private:
    std::map<std::string, std::unique_ptr<SerieBase>> series_;
};

// -----------------------------------------------------

} // namespace df

#include <dataframe/inline/Dataframe.hxx>
