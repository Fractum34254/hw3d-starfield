#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include <type_traits>
#include <memory>
#include <unordered_map>
#include <random>

namespace Bind
{
	class Codex
	{
	public:
		template<class T,typename...Params>
		static std::shared_ptr<T> Resolve( Graphics& gfx,Params&&...p ) noxnd
		{
			static_assert( std::is_base_of<Bindable,T>::value,"Can only resolve classes derived from Bindable" );
			return Get().Resolve_<T>( gfx,std::forward<Params>( p )... );
		}
		template<class T, typename...Params>
		static std::shared_ptr<T> ResolveUnique(Graphics& gfx, Params&&...p) noxnd
		{
			static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
			return Get().ResolveUnique_<T>(gfx, std::forward<Params>(p)...);
		}
	private:
		template<class T,typename...Params>
		std::shared_ptr<T> Resolve_( Graphics& gfx,Params&&...p ) noxnd
		{
			const auto key = T::GenerateUID( std::forward<Params>( p )... );
			const auto i = binds.find( key );
			if( i == binds.end() )
			{
				auto bind = std::make_shared<T>( gfx,std::forward<Params>( p )... );
				binds[key] = bind;
				return bind;
			}
			else
			{
				return std::static_pointer_cast<T>( i->second );
			}
		}
		template<class T, typename...Params>
		std::shared_ptr<T> ResolveUnique_(Graphics& gfx, Params&&...p) noxnd
		{
			std::random_device rd;
			std::mt19937 rng(rd());
			std::uniform_int_distribution<int> dist(0, 2147483647);

			std::string key = std::to_string(dist(rng));
			while (binds.find(key) != binds.end())
			{
				key = std::to_string(dist(rng));
			}
			auto bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
			binds[key] = bind;
			return bind;
		}
		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}
	private:
		std::unordered_map<std::string,std::shared_ptr<Bindable>> binds;
	};
}