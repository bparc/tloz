#include "state.h"
#include "draw.c"

static inline void Intersect(game_state_t* state, game_object_t* Generic, v2_t N, int32_t flags)
{
	if (!Generic->WasRemoved)
	{
		switch (Generic->Type)
		{
		case ENT_PROJECTILE_OCTOROCK:
		{
			Remove(Generic);
		} break;
		case ENT_PLAYER:
		{
			if (((flags & INTERSECT_OUT_OF_BOUNDS) && (N.y == 0)))
			{
				if ((Generic->Facing.x == -N.x) && ((Generic->Y - state->Map.DoorAlign[1]) == 8))
				{
					RoomTransition(state);
				}
				else
				{
					// ((entity->X - state->Map.DoorAlign[0]) == 8)
					// RoomTransition(state);
				}
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
				if (state->Player.Facing.x > 0)
				{
					state->Player.X = ( - state->Map.Extends.x + state->Player.Bounds.X1);
				}
				if (state->Player.Facing.x < 0)
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
				ent_player_t* entity = &state->Player._Player;
				entity->tAttack += dT * 4.0f;
				if (entity->IsAttacking && entity->tAttack >= 1.0f)
				{
					entity->IsAttacking = 0;
				}
				if (cons->A) // ATTACK
				{
					if (!entity->IsAttacking && (entity->tAttack >= 1.5f))
						{
							entity->IsAttacking = TRUE;
							entity->tAttack = 0.0f;
							sword_t* Sword = &state->Sword;
							Sword->Bounds = (bb_t){ 0,0,0,0 };
							if (entity->Facing.x > 0)
							{
								Sword->Bounds = BoundsFromRectangle(entity->X + 6, entity->Y, 16, 2);
							}
							else
							if (entity->Facing.x < 0)
							{
								Sword->Bounds = BoundsFromRectangle(entity->X - 22, entity->Y, 16, 2);
							}
							else
							if (entity->Facing.y > 0)
							{
								Sword->Bounds = BoundsFromRectangle(entity->X, entity->Y + 7, 2, 16);
							}
							else
							if (entity->Facing.y < 0)
							{
								Sword->Bounds = BoundsFromRectangle(entity->X - 2, entity->Y - 24, 2, 16);
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
				if (!entity->IsAttacking)
				{
					MoveI(state, &state->Player, cons->DPad, 60, dT, MOVE_ALIGN);
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
#if 1
			// ENTITIES
			for (int32_t index = 0; index < state->EntityCount; index++)
			{
				game_object_t* Generic = state->Entities + index;
				if (!Generic->WasRemoved)
				{
					switch (Generic->Type)
					{
					case ENT_OCTOROCK:
					{
						ent_octorock_t* entity = &Generic->_Octorock;
						entity->tChangeFacingDir += dT;
						if (entity->tChangeFacingDir >= 1.0f)
						{
							entity->Facing = PickRandomCardinalDir();
							entity->tChangeFacingDir = 0.0f;
						}
						entity->tShoot += dT;
						if (entity->tShoot >= 1.0f)
						{
							CreateProjectile(state, entity->X, entity->Y, entity->Facing);
							entity->tShoot = 0.0f;
						}
						MoveI(state, Generic, Generic->Facing, 20, dT, MOVE_ALIGN);
					} break;
					case ENT_KEESE:
					{
						ent_keese_t* entity = &Generic->_Keese;
						entity->tChangeFacingDir += dT * 0.5f;
						if (entity->tChangeFacingDir >= 1.0f)
						{
							entity->Facing = PickRandomDir();
							entity->tChangeFacingDir = 0.0f;
						}
						MoveI(state, Generic, Generic->Facing, 20, dT, MOVE_DO_NOT_INTERSECT);
					} break;
					case ENT_TEKTITE:
					{
						ent_tektite_t* entity = &Generic->_Tektite;
						entity->tJump += dT * 1.0f;
						if (entity->tJump >= 1.0f)
						{
							if (!entity->HasJumped)
							{
								entity->From = V2(entity->X, entity->Y);
								entity->To = V2(entity->From.x + (rand() % 2 ? 64 : -64), entity->From.y);
								entity->HasJumped = TRUE;
							}
							float T = (entity->tJump - 1.0f);
							{
								entity->X = Lerp(entity->From.x, entity->To.x, T);
								entity->Y = (int32_t)(HalfArc(T * 1.0f) * 20.0f) + entity->From.y;
							}
							if (T >= 1.0f)
							{
								SetPosition(Generic, entity->To);
								entity->tJump = 0.0f;
								entity->HasJumped = 0;
							}
							entity->X = ClampI(entity->X, 0, state->Map.Extends.x);
						}
					} break;
					case ENT_ZORA:
					{

					} break;
					case ENT_PROJECTILE_OCTOROCK:
					{
						MoveI(state, Generic, Generic->Facing, 150, dT, 0);
					} break;
					case ENT_SPAWN:
					{
						ent_spawn_t* entity = &Generic->_Spawn;
						entity->tRemaining += dT * 2.0f;
						if (entity->tRemaining >= 1.0f)
						{
							game_object_t* result = spawn(state, entity->X, entity->Y, entity->EnemyType);
							result->Flags = (ENT_FLAGS_ENEMY | ENT_FLAGS_CAN_DAMAGE);
							result->Facing = PickRandomCardinalDir();
							result->Bounds = BB(-8, -8, +8, +8);
							Remove(Generic);
						}
					} break;
					case ENT_PARTICLE_EFFECT:
					{
						ent_particle_t* entity = &Generic->_Particle;
						entity->tRemaining += dT * 2.0f;
						if (entity->tRemaining >= 1.0f)
						{
							Remove(Generic);
						}
					} break;
					}
				}
				if (Generic->WasRemoved)
				{
					state->Entities[index--] = state->Entities[--state->EntityCount];
				}
			}
#endif
		}
		//NOTE: Rendering
		_DrawFrame(framebuffer, state, RAM,time);
		state->PrevTime = time;
	}
	return result;
}