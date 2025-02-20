// #include "Q-learning.h"

// Q_Learning::Q_Learning()
// {
// }

// void Q_Learning::Generation()
// {
//     GpuNeuralNetwork* neuralNetworks = (GpuNeuralNetwork*)malloc(BATCH_SIZE * sizeof(GpuNeuralNetwork));
//     for(int i = 0; i < SURVIVOR_NUM; i++)
//     {
//         new (&neuralNetworks[i]) GpuNeuralNetwork(agents[i]);
//     }
//     for(int i = SURVIVOR_NUM; i < BATCH_SIZE; i++)
//     {
//         new (&neuralNetworks[i]) GpuNeuralNetwork(agents[i % SURVIVOR_NUM]);
//     }


//     size_t num_threads =  BATCH_SIZE - 1;
//     pthread_t* threads = (pthread_t* )malloc(sizeof(pthread_t) * num_threads);
//     for(int i = 0; i < num_threads; i++)
//     {
//         pthread_create(&threads[i], nullptr, simulate, (void*)&neuralNetworks[i]);
//     }
//     // Make the main thread also work!
//     evals[BATCH_SIZE - 1] = simulate(&neuralNetworks[BATCH_SIZE - 1], true);
//     for(int i = 0; i < num_threads; i++)
//     {
//         float* ret;
//         pthread_join(threads[i], (void**)&ret);
//         evals[i] = *ret;
//         free(ret);
//     }
//     free(threads);
// }

// float simulate(GpuNeuralNetwork *nn)
// {
//     float* evals = (float*)malloc(NUM_SIMULATIONS_PER_AGENT * sizeof(float));

//     for(int simulation = 0; simulation < NUM_SIMULATIONS_PER_AGENT; simulation++)
//     {
//         Polygon a(ground), b(right_wall), c(left_wall), d(rocket), e(target), f(wall);
//         Object* objects[] = {&a, &b, &c, &d, &e, &f};
//         f.SetPosition({f.ReturnPosition().x, -250});
//         d.SetPosition({(float)(20 + (rand() % (WIDTH - 20 - 20))), d.ReturnPosition().y});

//         if(exportScene)
//         {
//             current_scene_objects = objects;
//             num_objects = sizeof(objects) / sizeof(Object*);
//         }
        
//         PlayerData player_data;

//         PositionData position_data = {
//             .reward = -INFINITY,
//             .starting_eval = GetEval(((Polygon*)objects[3])->ReturnPosition(), ((Polygon*)objects[4])->ReturnPosition(), true, false),
//             .num = 0,
//             .inputs = (float*)malloc(INPUT_HEIGHT * INPUT_WIDTH * NUM_INPUT_LAYERS * DURATION_SEC * (1.f / DELTA_TIME) * sizeof(float)),
//             .outputs = (float*)malloc(NUM_OUTPUT * DURATION_SEC * (1.f / DELTA_TIME) * sizeof(float))
//         };

//         float time;
//         for(time = 0; time < DURATION_SEC; time+=DELTA_TIME)
//         {
//             // DRAW SCREEN
//             #ifdef PRINT_TIME
//             TimeSetZero();
//             #endif
//             float* input = (float*)malloc(NUM_INPUT_LAYERS * INPUT_HEIGHT * INPUT_WIDTH * sizeof(float));
//             for(int i = 0; i < NUM_INPUT_LAYERS * INPUT_HEIGHT * INPUT_WIDTH; i++)
//                 input[i] = 0;
//             //input[NUM_INPUT_LAYERS * INPUT_HEIGHT * INPUT_WIDTH] = FLT_MAX;

//             // OBSTACLE LAYER   
//             for(int i = 0; i < sizeof(objects) / sizeof(Object*); i++)
//             {
//                 if(((Polygon*)objects[i])->ReturnCollisionLayer() == OBSTACLE_LAYER)
//                 {
//                     ((Polygon*)objects[i])->DrawInArray(input + (OBSTACLE_LAYER_INPUT_ID * INPUT_HEIGHT * INPUT_WIDTH), HEIGHT, WIDTH, INPUT_HEIGHT, INPUT_WIDTH);
//                 }
//                 else if(((Polygon*)objects[i])->ReturnCollisionLayer() == RESTING_LAYER)
//                 {
//                     ((Polygon*)objects[i])->DrawInArray(input + (RESTING_LAYER_INPUT_ID * INPUT_HEIGHT * INPUT_WIDTH), HEIGHT, WIDTH, INPUT_HEIGHT, INPUT_WIDTH);
//                 }
//                 else if(((Polygon*)objects[i])->ReturnCollisionLayer() == DESTINATION_LAYER)
//                 {
//                     ((Polygon*)objects[i])->DrawInArray(input + (DESTINATION_LAYER_INPUT_ID * INPUT_HEIGHT * INPUT_WIDTH), HEIGHT, WIDTH, INPUT_HEIGHT, INPUT_WIDTH);
//                 }
//                 else if(((Polygon*)objects[i])->ReturnCollisionLayer() == ROCKET_LAYER)
//                 {
//                     ((Polygon*)objects[i])->DrawInArray(input + (ROCKET_INPUT_ID * INPUT_HEIGHT * INPUT_WIDTH), HEIGHT, WIDTH, INPUT_HEIGHT, INPUT_WIDTH);
//                 }
//             }
//             if(time != 0)
//                 memcpy(input + (ROCKET_PREVIOUS_INPUT_ID * INPUT_HEIGHT * INPUT_WIDTH), previous_frame, INPUT_HEIGHT * INPUT_HEIGHT * sizeof(float));
//             memcpy(previous_frame, input + (ROCKET_INPUT_ID * INPUT_HEIGHT * INPUT_WIDTH), INPUT_HEIGHT * INPUT_HEIGHT * sizeof(float));

//             #ifdef PRINT_TIME
//             TimeElapsed("Input setup: ");
//             #endif
//             float* output = nn->Generate(input, false);
//             #ifdef PRINT_TIME
//             TimeElapsed("Generate input: ");
//             #endif
//             //float* output = nullptr;
            
//             // printf("%f %f\n", output[OUT_MOVE_RIGHT_ID], output[OUT_MOVE_LEFT_ID]);
//             // rocket_ptr->ApplyAccel({25, 50}, {GRAVITY * min(max((output[OUT_MOVE_RIGHT_ID]), 0), 1), -GRAVITY * min(max((output[OUT_MOVE_RIGHT_ID]), 0), 1)}, DELTA_TIME);
//             // rocket_ptr->ApplyAccel({0, 50}, {-GRAVITY * min(max((output[OUT_MOVE_LEFT_ID]), 0), 1), -GRAVITY * min(max((output[OUT_MOVE_LEFT_ID]), 0), 1)}, DELTA_TIME);
//             ((Polygon*)objects[3])->ApplyAccel({25, 50}, {GRAVITY * output[OUT_MOVE_RIGHT_ID], -GRAVITY * output[OUT_MOVE_RIGHT_ID]}, DELTA_TIME);
//             ((Polygon*)objects[3])->ApplyAccel({0, 50}, {-GRAVITY * output[OUT_MOVE_LEFT_ID], -GRAVITY * output[OUT_MOVE_LEFT_ID]}, DELTA_TIME);
            

//             //if(output)

//             // SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
//             // SDL_RenderClear(renderer);

//             // for(int i = 0; i < num_objects; i++)
//             // {
//             //     objects_ptr[i]->Draw(win);
//             // }

//             // SDL_RenderPresent(renderer);
//             // SDL_UpdateWindowSurface(win);

//             // //DECIDE CHOICE
//             // SDL_Surface* windowSurface = SDL_GetWindowSurface(win);
//             // windowSurface->pixels;//pixles :)


//             //UPDATE
//             for(int i = 0; i < sizeof(objects) / sizeof(Object*); i++)
//             {
//                 objects[i]->Update(DELTA_TIME);
//                 CollisionsData data =((Polygon*)objects[i])->CheckForCollision(objects, sizeof(objects) / sizeof(Object*), DELTA_TIME);
//                 bool in_contact_with_ground = false;
//                 if(data.num > 0)
//                 {
//                     for(int i = 0; i < data.num; i++)
//                     {
//                         if(data.data[i].collision_layer_1 == ROCKET_LAYER || data.data[i].collision_layer_2 == ROCKET_LAYER)
//                         {
//                             if(data.data[i].collision_layer_1 == OBSTACLE_LAYER || data.data[i].collision_layer_2 == OBSTACLE_LAYER)
//                             {
//                                 player_data.collided = true;
//                                 free(output);
//                                 free(input);
//                                 free(data.data);
//                                 goto EVAL;
//                             }   
//                             else if(data.data[i].collision_layer_1 == DESTINATION_LAYER || data.data[i].collision_layer_2 == DESTINATION_LAYER)
//                             {
//                                 player_data.succeded = true;
//                                 free(output);
//                                 free(input);
//                                 free(data.data);
//                                 goto EVAL;
//                             }
//                             else if(data.data[i].collision_layer_1 == RESTING_LAYER || data.data[i].collision_layer_2 == RESTING_LAYER)
//                             {
//                                 if(!player_data.lifted_off)
//                                     in_contact_with_ground = true;
//                                 else{
//                                     player_data.collided = true;
//                                     free(output);
//                                     free(input);
//                                     free(data.data);
//                                     goto EVAL;
//                                 }
//                             }
//                         }
//                     }
//                 }
//                 free(data.data);
//                 if(!in_contact_with_ground)
//                     player_data.lifted_off = true;
//             }
//             float eval = GetEval(((Polygon*)objects[3])->ReturnPosition(), ((Polygon*)objects[4])->ReturnPosition(), true, false);
//             if(eval > player_data.max_eval)
//                 player_data.max_eval = eval;

//             #ifdef PRINT_TIME
//             TimeElapsed("Physics: ");
//             #endif
//             // d_eval = eval - (time == 0 ? eval : last_eval);
//             // d2_eval = d_eval - (time == 0 ? d_eval : last_d_eval);

//             // float* corrected_output = (float*)malloc(2 * sizeof(float));
//             // for(int i = 0; i < 2;i++)
//             //     corrected_output[i] = output[i] + (output[i] * FastSigmoid(d2_eval, 0.7f));

//             memcpy(position_data.inputs + (INPUT_HEIGHT * INPUT_WIDTH * NUM_INPUT_LAYERS * position_data.num), input, INPUT_HEIGHT * INPUT_WIDTH * NUM_INPUT_LAYERS * sizeof(float));
//             memcpy(position_data.outputs + (NUM_OUTPUT * position_data.num), output, NUM_OUTPUT * sizeof(float));
//             position_data.num++;

//             // last_eval = eval;
//             // last_d_eval = d_eval;
//             free(output);
//             free(input);
//         }

//     EVAL:
//         float eval = GetEval(((Polygon*)objects[3])->ReturnPosition(), ((Polygon*)objects[4])->ReturnPosition(), player_data.collided, player_data.succeded);

//         eval += player_data.max_eval;
//         // eval -= abs(((Polygon*)objects[3])->ReturnRotation()) * RADIAN;
//         if(player_data.succeded)
//         eval -= time;
//         else
//             ;// eval -= time;

//         // printf("Eval: %f\n", eval);
//         if(exportScene)
//         {
//             num_objects = 0;
//             pthread_mutex_lock(&p_mutex);
//             pthread_kill(play_thread_id, SIGHOLD);
//             pthread_cond_wait(&has_stopped, &p_mutex);
//             pthread_mutex_unlock(&p_mutex);
//         }
//         position_data.reward = eval;

//         if(player_data.succeded)
//             positions.AddPositionData(position_data);
//         else
//         {
//             free(position_data.inputs);
//             free(position_data.outputs);
//         }

//         evals[simulation] = eval;
//     }
//     // printf("Eval ended!\n");
//     float eval = 0;
//     for(int i = 0; i < NUM_SIMULATIONS_PER_AGENT; i++)
//         eval += evals[i];
//     eval /= NUM_SIMULATIONS_PER_AGENT;
//     free(evals);

//     return eval;
// }