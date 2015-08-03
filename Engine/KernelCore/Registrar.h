#ifndef _VE_REGISTRAR_
#define _VE_REGISTRAR_
#include <map>
#include <functional>

template<typename ID, class T>
class veRegistrar
{
public:

	~veRegistrar(){};

	static veRegistrar<ID, T>* instance(){
		static veRegistrar<ID, T> registrar;
		return &registrar;
	}

	bool reg(const ID id, const T *t){
		auto iter = _registry.find(id);
		if (iter != _registry.end()) return false;
		_registry[id] = t;
		if (!_registerCallbacks.empty()){
			for (auto &iter : _registerCallbacks){
				iter(id, t);
			}
		}
	}

	bool unReg(const ID id){
		auto iter = _registry.find(id);
		if (iter != _registry.end()){
			if (!_unRegisterCallbacks.empty()){
				for (auto &iter : _unRegisterCallbacks){
					iter(id, t);
				}
			}
			_registry.erase(iter);
			return true;
		}
		return false;
	}

	T* getRegContent(const ID id){
		auto iter = _registry.find(id);
		if (iter != _registry.end()){
			return iter->second;
		}
		return nullptr;
	}

	const std::map<ID, T*>& getRegistry() const { return _registry; }

	void addRegisterCallback(const std::function<void(ID, T*)> &func) { _registerCallbacks.push_back(func); }
	void addUnRegisterCallback(const std::function<void(ID, T*)> &func) { _unRegisterCallbacks.push_back(func); }

protected:

	veRegistrar(){};

private:

	std::map<ID, T*> _registry;
	std::vector< std::function<void(ID, T*)> > _registerCallbacks;
	std::vector< std::function<void(ID, T*)> > _unRegisterCallbacks;
};

#endif