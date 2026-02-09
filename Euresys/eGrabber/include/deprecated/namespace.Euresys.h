#ifdef EURESYS_USE_NS_EURESYS_DEPRECATED_API

/** @addtogroup GenApi GenApi
    Helper functions to build Euresys GenApi queries

    The functions below create queries to list features or enumeration entries
    of features from a GenApi module

    @note these functions do not return actual information but do return
    queries for EGrabber::getStringList
    @note a query string is a Euresys custom extension only supported the
    Euresys GenApi module
    @{
 **/

/** Create a query to get the list of features exposed by a GenApi Module
    @param availableOnly if true: the query will be configured to only include
    features available at "query" time; if false: the query will be configured
    to include all the exposed features
    
    Example: get names of all available features exposed by the camera
    @code{.cpp}
    vector<string> cameraFeatures(grabber.getStringList<RemoteModule>(Features()));
    @endcode
 **/
std::string Features(bool availableOnly = true);
/** Create a query to get the list of features exposed by a GenApi Module
    and matching a given regular expression pattern
    @param re regular expression matching feature names
    @param availableOnly if true: the query will be configured to only include
    matching features available at "query" time; if false: the query will be
    configured to include all matching exposed features
    
    Example: get names of all features starting with "Cxp"
    @code{.cpp}
    vector<string> cameraFeatures(grabber.getStringList<RemoteModule>(RegexFeatures("^Cxp")));
    @endcode
 **/
std::string RegexFeatures(const std::string &re, bool availableOnly = true);
/** Create a query to get the list of features exposed by a GenApi Module
    and matching a given glob pattern
    @param glob glob pattern matching feature names
    @param availableOnly if true: the query will be configured to only include
    matching features available at "query" time; if false: the query will be
    configured to include all matching exposed features
    @note Available glob pattern wildcards:
      - @b * matches 0, 1 or several characters
        e.g. "*Max" could match "WidthMax", "HeightMax", "Max"
      - @b ? matches exactly 1 character
        e.g. "offset?" could match "offsetX", "offsetY"

    Example: get names of all features with name ending with "Max"
    @code{.cpp}
    vector<string> maxFeatures(grabber.getStringList<RemoteModule>(GlobFeatures("*Max")));
    @endcode
 **/
std::string GlobFeatures(const std::string &glob, bool availableOnly = true);
/** Create a query to get the list of entries of a GenApi enumeration feature
    @param feature name of enumeration feature to query
    @param availableOnly if true: the query will be configured to only include
    enumeration entries available at "query" time; if false: the query will be
    configured to include all the enumeration entries of the given feature

    Example: get all enumeration entries of PixelFormat, including those which
    are not available at "query" time
    @code{.cpp}
    vector<string> allFormats(grabber.getStringList<RemoteModule>(EnumEntries("PixelFormat", false)));
    @endcode
 **/
std::string EnumEntries(const std::string &feature, bool availableOnly = true);
/** Create a query to get the list of entries of a GenApi enumeration feature
    and matching a given regular expression pattern
    @param feature name of enumeration feature to query
    @param re regular expression matching enumeration entry names
    @param availableOnly if true: the query will be configured to only include
    matching enumeration entries available at "query" time; if false: the query
    will be configured to include all matching enumeration entries of the given
    feature

    Example: get all available enumeration entries of PixelFormat starting with
    "Mono"
    @code{.cpp}
    vector<string> monoFormats(grabber.getStringList<RemoteModule>(RegexEnumEntries("PixelFormat", "^Mono")));
    @endcode
 **/
std::string RegexEnumEntries(const std::string &feature, const std::string &re, bool availableOnly = true);
/** Create a query to get the list of entries of a GenApi enumeration feature
    and matching a given glob pattern
    @param feature name of enumeration feature to query
    @param glob glob pattern matching enumeration entry names
    @param availableOnly if true: the query will be configured to only include
    matching enumeration entries available at "query" time; if false: the query
    will be configured to include all matching enumeration entries of the given
    feature
    @note Available glob pattern wildcards:
      - @b * matches 0, 1 or several characters
        e.g. "Mono*" could match "Mono8", "Mono16"
      - @b ? matches exactly 1 character
        e.g. "Mono1?" could match "Mono10", "Mono12", "Mono14", "Mono16"

    Example: get all available enumeration entries of PixelFormat containing
    with "BGR"
    @code{.cpp}
    vector<string> bgrFormats(grabber.getStringList<RemoteModule>(GlobEnumEntries("PixelFormat", "*BGR*")));
    @endcode
 **/
std::string GlobEnumEntries(const std::string &feature, const std::string &glob, bool availableOnly = true);

#endif
