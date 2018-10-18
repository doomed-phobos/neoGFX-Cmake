// rectangle.cpp
/*
  neogfx C++ GUI Library
  Copyright (c) 2015 Leigh Johnston.  All Rights Reserved.
  
  This program is free software: you can redistribute it and / or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <neogfx/neogfx.hpp>
#include <neogfx/game/rectangle.hpp>
#include <neogfx/game/ecs_helpers.hpp>

namespace neogfx::game
{
	namespace shape
	{
		rectangle::rectangle(i_ecs& aEcs, const vec3& aPosition, const vec2& aExtents) :
			entity{ aEcs, archetype(aEcs).id() }
		{
			game::rectangle::meta::update(game::rectangle{ aPosition, aExtents }, aEcs, id());
		}

		rectangle::rectangle(i_ecs& aEcs, const vec3& aPosition, const vec2& aExtents, const neogfx::colour& aColour) :
			entity{ aEcs, archetype(aEcs).id() }
		{
			game::rectangle::meta::update(game::rectangle{ aPosition, aExtents }, aEcs, id());
			aEcs.component<material>().populate(id(), material{ colour{ aColour.to_vec4() }, {}, {} });
		}

		rectangle::rectangle(i_ecs& aEcs, const vec3& aPosition, const vec2& aExtents, const i_texture& aTexture) :
			entity{ aEcs, archetype(aEcs).id() }
		{
			game::rectangle::meta::update(game::rectangle{ aPosition, aExtents }, aEcs, id());
			aEcs.component<material>().populate(id(), material{ {}, {}, {}, to_ecs_component(aTexture) });
		}

		rectangle::rectangle(i_ecs& aEcs, const vec3& aPosition, const vec2& aExtents, const i_image& aImage) :
			entity{ aEcs, archetype(aEcs).id() }
		{
			game::rectangle::meta::update(game::rectangle{ aPosition, aExtents }, aEcs, id());
			aEcs.component<material>().populate(id(), material{ {}, {}, {}, to_ecs_component(aImage) });
		}

		rectangle::rectangle(i_ecs& aEcs, const vec3& aPosition, const vec2& aExtents, const i_texture& aTexture, const rect& aTextureRect) :
			entity{ aEcs, archetype(aEcs).id() }
		{
			game::rectangle::meta::update(game::rectangle{ aPosition, aExtents }, aEcs, id());
			aEcs.component<material>().populate(id(), material{ {}, {}, {}, to_ecs_component(aTexture, aTextureRect) });
		}

		rectangle::rectangle(i_ecs& aEcs, const vec3& aPosition, const vec2& aExtents, const i_image& aImage, const rect& aTextureRect) :
			entity{ aEcs, archetype(aEcs).id() }
		{
			game::rectangle::meta::update(game::rectangle{ aPosition, aExtents }, aEcs, id());
			aEcs.component<material>().populate(id(), material{ {}, {}, {}, to_ecs_component(aImage, aTextureRect) });
		}

		rectangle::rectangle(const rectangle& aOther) :
			entity{ aOther.ecs(), archetype(aOther.ecs()).id() }
		{
			// todo: clone entity
		}
	}
}