#ifndef DEF_LOCATION_HPP
# define DEF_LOCATION_HPP
# include "./Pair.hpp"
# include "./redirectRules.hpp"
# include <algorithm>

struct CGI
{
	std::string extension;
	std::string path;
};

struct Upload
{
	bool enabled;
	std::string store_path;
};

class BaseServerConfig
{
	protected:
		/**
		 * Redirect rules for this location, if any
		 * The key is the status code, the value is the URL to redirect to
		 * @key `return`
		 */
		std::vector<redirectRule>	_redirect_rules;

		std::vector<redirectRule> 	_errors_redirect;
		/**
		 * Path to the directory where the files are located
		 */
		std::string _root;
		/**
		 * Path to the index file
		 */
		std::vector<std::string> _index;
		/**
		 * Defined with `on` or `off` in the config file
		 * determine if the server should index directory files
		 */
		bool _auto_index;

		

		void	push_redirect_error(unsigned int code, std::string path);
		void	push_redirect_rule(unsigned int code, std::string path);

	public:
		BaseServerConfig();
		virtual ~BaseServerConfig();

		/** Operators **/
		BaseServerConfig &operator=(const BaseServerConfig &other);

		unsigned int	_errors_redirect_size;

		/** SETTERS **/

		void setRedirectRules(Pair *location_child_value);
		void setErrorsRedirect(Pair *server_child_value);
		void setRoot(Pair *location_child_value);
		void setIndex(Pair *location_child_value);
		void setAutoIndex(Pair *location_child_value);

		/** GETTERS **/

		std::vector<redirectRule> getRedirectRules() const;
		std::vector<redirectRule> *getErrorsRedirect();
		std::string getRoot() const;
		std::vector<std::string> getIndex() const;
		bool getAutoindex() const;
};

class Location : public BaseServerConfig
{

private:
	/**
	 * Path to the directory where the files are located
	*/
	std::string _path;
	/**
	 * List of all the methods allowed for this location
	 * Available methods are `GET`, `POST`, `DELETE`
	 */
	std::vector<std::string> _allowed_methods;
	
	/**
	 * List of all the CGI extensions for this location
	 * The key is the extension, the value is the path to the CGI script
	 */
	std::vector<CGI> _cgi_list;
	/**
	 * Specifies a directory to which output files will be saved to.
	 * Also enables the upload feature
	 */
	Upload _upload;

	void handle_child_value(Pair *current_pair);
	void pushCGI(std::string extension, std::string path);
	void initUpload();

public:
	Location(void);
	Location(Pair *location_config, std::string location_path);
	~Location();
	Location &operator=(const Location &other);

	/** SETTERS **/

	void setPath(std::string location_path);
	void setAllowedMethods(Pair *location_child_value);
	void setCGI(Pair *location_child_value);
	void setUploadStore(Pair *location_child_value);
	void setUploadEnable(Pair *location_child_value);

	/** GETTERS **/

	std::string getPath() const;
	std::vector<std::string> getAllowedMethods() const;
	std::vector<CGI> getCGI() const;
	std::string getCGIPath(std::string extension) const;
	bool getUploadEnabled() const;
	std::string getUploadStore() const;
};

#endif