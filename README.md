# Wizrd

Wizrd aims to be a modern C++ Web Microframework in the same molds as Flask is to Python.

```C++
#include <wizrd>

using namespace Wizrd;

APP(MyApp) {
    Route("/") {
		request.get = []() {
			return "hello world!";
		}
	}

	Route("/login") {
		request.accept("application/json");
		request.get = [] () {
			std::string userName = request.form["user_name"];
			std::string password = request.form["password"];
			return LoginService::login(userName, password).toJson();
		}
	}
}
Register(MyApp);
runApps();
```
