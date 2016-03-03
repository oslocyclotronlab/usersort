// -*- c++ -*-

#ifndef Parameters_H_
#define Parameters_H_ 1

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

// ########################################################################

class Parameters;

//! A list of floats.
/*! The list can be set from text. The size of the list is variable.
 */
class Parameter {
public:
    //! The type of the list elements.
    typedef float param_t;

    //! Construct a parameter object and register with the parameter list.
    /*! If the standard size of the list is >0, a warning will be
     *  printed if setting to a different size.
     */
    Parameter(Parameters& plist,       /*!< The parameter list to register with. */
              const std::string& name, /*!< The name of this parameter. */
              int std_size=-1,         /*!< The standard size of the list. */
              param_t dflt_value=0     /*!< The default value to set if a std_size is given. */ );

    //! Deregister from the parameter list.
    ~Parameter();

    //! Obtain the size of the list.
    /*! \return The list's size. */
    unsigned int GetSize() const
        { return values.size(); }

    //! Obtain the standard size of the list.
    /*! \return The list's standard size. */
    unsigned int GetStandardSize() const
        { return std_size; }

    //! Set a list element.
    /*! If the index is too large, nothing is done.
     */
    void Set(unsigned int idx, /*!< The index of the value in the list that is to be changed. */
             param_t value     /*!< The new value. */)
        { if( idx<GetSize() ) values[idx] = value; }

    //! Set the whole list.
    void Set(const std::vector<param_t>& values /*!< The list of new values. */ );

    //! Set the values from text.
    /*! Extracts new values from the text.
     */
    void Set(const std::string& values_txt /*!< The text with the new values. */ );

    //! Retrieve a value.
    /*! \return the value at the given index, or 0 if the index is too large.
     */
    param_t Get(unsigned int idx /*!< The index if the value to retrieve. */ ) const
        { return idx>=GetSize() ? 0 : values[idx]; }

    //! Operator notation for Get().
    param_t operator[](unsigned int idx) const
        { return Get(idx); }

    //! Evaluate the list as polynomial.
    /*! \return The value of the polynomial at x.
     */
    param_t Poly(param_t x /*!< Where the polynomial should be evaluated. */ ) const;

    //! Evaluate part of the list as polynomial.
    /*! \return The value of the polynomial at x.
     */
    param_t Poly(param_t x,             /*!< Where the polynomial should be evaluated.  */
                 unsigned int startidx, /*!< The first index for polynomial evaluation. */
                 unsigned int count     /*!< The degree of the polynomial minus 1. */) const;

private:
    //! The parameter list that this parameter is registered to.
    Parameters& parameters;

    //! The parameter's name.
    std::string name;

    //! The standard size of the parameter list.
    int std_size;

    //! The values of this Parameter object.
    std::vector<param_t> values;
};

// ########################################################################

//! A mapping of names to Parameter objects.
class Parameters {
public:

    //! Find a parameter by name.
    /*! \return the parameter, if known, or 0 otherwise. */
    Parameter* Find(const std::string& name /*!< The parameter to find. */);

    //! Add a parameter to the mapping.
    /*! No warning is given if a parameter with the same name already
     *  exists. The mapping does not take ownership of the Parameter
     *  object and will not destroy it.
     */
    void Add(const std::string& name, /*!< The name of the parameter to add. */
             Parameter* param         /*!< The parameter to add. */);

    //! Remove a parameter from the mapping.
    void Remove(const std::string& name /*!< The name of the parameter to remove. */);

    //! Remove a parameter from the mapping.
    void Remove(Parameter* param /*!< The parameter to remove. */);

    //! Set one or more parameters from text.
    /*! The format for the text is:
     *  <pre>
     *  &lt;parameter-name&gt; = &lt;values&gt; [ ; &lt;parameter-name&gt; = &lt;values&gt; ]*
     *  </pre>
     *
     *  \return true if the text was parsed completely; if false, some
     *  parameters still might have been set before the error was
     *  encountered
     */
    bool SetAll(std::istringstream& icmd /*!< The parameter description to read. */);

private:
    //! The map type used by this class.
    typedef std::map<std::string, Parameter*> names_t;

    //! The map from name to parameter object.
    names_t names;
};

// ########################################################################

#endif /* Parameters_H_ */
