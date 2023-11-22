#include "state.h"
#include "draw.c"

static int32_t Host(framebuffer_t* framebuffer, chr_ram_t* RAM, void* memory,
	int32_t allocationSize,controller_t*cons,double time)
{
	int32_t result = EXIT_SUCCESS;
	assert(allocationSize >= sizeof(game_state_t));
	game_state_t* state = memory;
	if (_Init(state))
	{
		float dT = state->PrevTime ? (float)(time - state->PrevTime) : 0.0f;
		// NOTE: Transition
		if ((state->RequestedRoom != 0))
		{
			state->tTransit += dT * 1.0f;
			if (state->tTransit >= 1.5f)
			{
				LoadRoomFromData(state, state->RequestedRoom);
				state->RequestedRoom = NULL;
				if (state->Player.ExitDir.x > 0)
				{
					state->Player.X = ( - state->Map.Extends.x + state->Player.Bounds.X1);
				}
				if (state->Player.ExitDir.x < 0)
				{
					state->Player.X = ( + state->Map.Extends.x - state->Player.Bounds.X1);
				}
			}
		}
		// NOTE: Map
		if ((state->RequestedRoom == 0))
		{
			// PLAYER
			if (!state->Player.IsAttacking)
			{
				MoveAligned(state, &state->Player, cons->DPad, 60, dT);
				state->Player.tRecovery += dT;
				if ((cons->A && !state->Player.IsAttacking) &&
					(state->Player.tRecovery>=0.2f))
				{
					printf("Attack has begun!\n");
					game_object_t* entity = &state->Player;
					entity->IsAttacking = TRUE;
					entity->tAttack = 0.0f;
					sword_t* Sword = &state->Sword;
					Sword->Position = V2I(entity->X, entity->Y);
					if (entity->Facing.x > 0)
					{
						Sword->Position.x = entity->X + 8;
					}
					else
					if (entity->Facing.x < 0)
					{
						Sword->Position.x = entity->X - 24;
					}
					else
					if (entity->Facing.y > 0)
					{
						Sword->Position.x = entity->X - 2;
						Sword->Position.y = entity->Y + 8;
					}
					else
					if (entity->Facing.y < 0)
					{
						Sword->Position.x = entity->X - 2;
						Sword->Position.y = entity->Y - 24;
					}
					for (int32_t index = 0; index < state->EntityCount; index++)
					{
						game_object_t* B = state->Entities + index;
					}
				}
				if (state->Player.ExitedRoom)
				{
					RequestRoomTransition(state);
				}
			}
			if (state->Player.IsAttacking) 
			{
				state->Player.tAttack += dT * 4.0f;
				if (state->Player.tAttack >= 1.0f)
				{
					state->Player.IsAttacking = 0;
					state->Player.tRecovery = 0;
				}
			}
			// ENTITIES
			for (int32_t index = 0; index < state->EntityCount; index++)
			{
				game_object_t* entity = state->Entities + index;
				switch (entity->Type)
				{
				case ENT_OCTOROCK:
				{
					entity->tChangeMovingDir += dT;
					if (entity->tChangeMovingDir >= 1.0f)
					{
						entity->Facing = PickRandomCardinalDir();
						entity->tChangeMovingDir = 0.0f;
					}
					entity->tShoot += dT;
					if (entity->tShoot >= 1.0f)
					{
						SpawnBullet(state,entity->X,entity->Y,entity->Facing,0);
						entity->tShoot = 0.0f;
					}
					MoveAligned(state, entity, entity->Facing, 20, dT);
				} break;
				case ENT_BULLET:
				{
					MoveAligned(state, entity, entity->Facing, 150, dT);
					if (entity->MoveIntersection)
					{
						state->Entities[index--] = state->Entities[--state->EntityCount];
					}
				} break;
				case ENT_SPAWN:
				{
					entity->tSpawn += dT * 0.5f;
					if (entity->tSpawn >= 1.0f)
					{
						spawn(state, entity->X, entity->Y, entity->SpawnType);
						state->Entities[index--] = state->Entities[--state->EntityCount];
					}
				} break;
				}
			}
		}
		//NOTE: Rendering
		_DrawFrame(framebuffer, state, RAM,time);
		state->PrevTime = time;
	}
	return result;
}