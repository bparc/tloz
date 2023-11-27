#include "state.h"
#include "draw.c"

static inline void Intersect(game_state_t* state, game_object_t* entity, v2_t N, int32_t flags)
{
	if (!entity->Removed)
	{
		switch (entity->Type)
		{
		case ENT_BULLET:
		{
			Remove(entity);
		} break;
		case ENT_PLAYER:
		{
			if (((flags & INTERSECT_OUT_OF_BOUNDS) && (N.y == 0)))
			{
				if ((entity->Facing.x == -N.x) && ((entity->Y - state->Map.DoorAlign[1]) == 8))
				{
					RoomTransition(state);
				}
				else
				{
					// ((entity->X - state->Map.DoorAlign[0]) == 8)
				}
			}
		} break;
		case ENT_KEESE:
		{
			if (flags & INTERSECT_OUT_OF_BOUNDS)
			{
				entity->Facing = N;
				entity->tChangeMovingDir = 0.0f;
			}
		} break;
		case ENT_OCTOROCK:
		{
			if (flags & INTERSECT_OUT_OF_BOUNDS)
			{
				entity->Facing = N;
				entity->tChangeMovingDir = 0.0f;
			}
		} break;
		}
	}
}

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
			{
				if (cons->A)
				{
					if (!state->Player.IsAttacking && (state->Player.tRecovery >= 0.2f))
					{
						game_object_t* entity = &state->Player;
						entity->IsAttacking = TRUE;
						entity->tAttack = 0.0f;
						sword_t* Sword = &state->Sword;
						Sword->Bounds = (bb_t){ 0,0,0,0 };
						if (entity->Facing.x > 0)
						{
							Sword->Bounds = BoundsFromRectangle(entity->X + 8, entity->Y - 2, 16, 2);
						}
						else
						if (entity->Facing.x < 0)
						{
							Sword->Bounds = BoundsFromRectangle(entity->X - 26, entity->Y-2, 16, 2);
						}
						else
						if (entity->Facing.y > 0)
						{
							Sword->Bounds = BoundsFromRectangle(entity->X - 2, entity->Y + 8, 2, 16);
						}
						else
						if (entity->Facing.y < 0)
						{
							Sword->Bounds = BoundsFromRectangle(entity->X - 2, entity->Y - 26, 2, 16);
						}
						for (int32_t index = 0; index < state->EntityCount; index++)
						{
							game_object_t* B = state->Entities + index;
							if (B->Flags & ENT_FLAGS_ENEMY)
							{
								if (TestBounds(TransformedBoundsFromEntity(B), state->Sword.Bounds))
								{
									Remove(B);
									spawn(state, B->X, B->Y, ENT_PARTICLE_EFFECT);
								}
							}
						}
					}
				}
				if (!state->Player.IsAttacking)
				{
					Move(state, &state->Player, cons->DPad, 60, dT, MOVE_ALIGN);
				}
				state->Player.tRecovery += dT;
				if (state->Player.IsAttacking)
				{
					state->Player.tAttack += dT * 4.0f;
					if (state->Player.tAttack >= 1.0f)
					{
						state->Player.IsAttacking = 0;
						state->Player.tRecovery = 0;
					}
				}
				for (int32_t index = 0; index < state->EntityCount; index++)
				{
					game_object_t* B = state->Entities + index;
					if (B->Flags & ENT_FLAGS_ENEMY)
					{
						if (TestOverlap(&state->Player, B))
						{
							// ...
						}
					}
				}
			}
			// ENTITIES
			for (int32_t index = 0; index < state->EntityCount; index++)
			{
				game_object_t* entity = state->Entities + index;
				if (!entity->Removed)
				{
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
							CreateProjectile(state, entity->X, entity->Y, entity->Facing, 0);
							entity->tShoot = 0.0f;
						}
						Move(state, entity, entity->Facing, 20, dT, MOVE_ALIGN);
					} break;
					case ENT_KEESE:
					{
						entity->tChangeMovingDir += dT * 0.5f;
						if (entity->tChangeMovingDir >= 1.0f)
						{
							entity->Facing = PickRandomDir();
							entity->tChangeMovingDir = 0.0f;
						}
						Move(state, entity, entity->Facing, 20, dT, MOVE_DO_NOT_INTERSECT);
					} break;
					case ENT_TEKTITE:
					{
						entity->tJump += dT * 1.0f;
						if (entity->tJump >= 1.0f)
						{
							if (!entity->HasJumped)
							{
								entity->JumpFrom = V2(entity->X, entity->Y);
								entity->JumpTo = entity->JumpFrom;
								entity->JumpTo.x += (rand() % 2 ? 64 : -64);
								entity->HasJumped = TRUE;
							}
							float T = (entity->tJump - 1.0f);
							{
								entity->X = Lerp(entity->JumpFrom.x, entity->JumpTo.x, T);
								entity->Y = JumpArc(20.0f, T * 5.0f) + entity->JumpFrom.y;
							}
							if (T >= 1.0f)
							{
								SetPosition(entity, entity->JumpTo);
								entity->tJump = 0.0f;
								entity->HasJumped = 0;
							}
							entity->X = ClampI(entity->X, 0, state->Map.Extends.x);
						}
					} break;
					case ENT_BULLET:
					{
						Move(state, entity, entity->Facing, 150, dT, 0);
					} break;
					case ENT_SPAWN:
					{
						entity->tSpawn += dT * 2.0f;
						if (entity->tSpawn >= 1.0f)
						{
							game_object_t* result = spawn(state, entity->X, entity->Y, entity->SpawnType);
							result->Flags = ENT_FLAGS_ENEMY;
							result->Bounds = BB(-8, -8, +8, +8);
							result->Facing = PickRandomCardinalDir();
							entity->Removed = TRUE;
						}
					} break;
					case ENT_PARTICLE_EFFECT:
					{
						entity->tSpawn += dT * 2.0f;
						if (entity->tSpawn >= 1.0f)
						{
							entity->Removed = TRUE;
						}
					} break;
					}
				}
				if (entity->Removed)
				{
					state->Entities[index--] = state->Entities[--state->EntityCount];
				}
			}
		}
		//NOTE: Rendering
		_DrawFrame(framebuffer, state, RAM,time);
		state->PrevTime = time;
	}
	return result;
}