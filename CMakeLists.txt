cmake_minimum_required(VERSION 3.20.2)
project(course_work)

set(CMAKE_CXX_STANDARD 20)

add_executable(course_work
        main.cpp
        db/data_base.h db/data_base.cpp
        db_user_communication.h db_user_communication.cpp
        db_value/db_value.h db_value/db_value.cpp
        db_value/db_value_builder.h db_value/db_value_builder.cpp
        db_value/string_holder.h
        db_key/key.h
        chain_of_resp_and_command/command.h
        chain_of_resp_and_command/handler.h
        logger/logger.h logger/logger.cpp
        logger/logger_holder.h logger/logger_holder.cpp
        logger/logger_builder.h logger/logger_builder.cpp
        allocator/memory_base_class.h allocator/memory_base_class.cpp
        allocator/memory_holder.h allocator/memory_holder.cpp
        allocator_from_global_heap/memory_from_global_heap.h allocator_from_global_heap/memory_from_global_heap.cpp
        b_tree/associative_container.h
        b_tree/b_tree.h
        )