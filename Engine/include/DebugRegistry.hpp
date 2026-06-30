#ifndef DEBUG_REGISTRY
#define DEBUG_REGISTRY

#include <string>
#include <functional>
#include <unordered_map>

// Members provide the DebugRegistry with getter lambdas to the data they want to expose.
// The DebugRegistry then accesses and renders all members' exposed data in a single call to renderMemberData().

class DebugRegistry {
	private:
		std::unordered_map<std::string, std::function<void()>> members;

	public:
		void registerMember(std::string member_name, std::function<void()> getter);
		void removeMember(std::string& member_name);
		void renderMemberData();
};

#endif