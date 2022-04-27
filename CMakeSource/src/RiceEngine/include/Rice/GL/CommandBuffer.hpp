/*
 * CommandBuffer.hpp
 *
 *  Created on: 26 April 2022
 *      Author: FileBinsLapTop
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

class CommandBuffer;
typedef SmartPtr<CommandBuffer> pCommandBuffer;

struct CommandBuffer_API_data;
typedef SmartPtr<CommandBuffer_API_data> pCommandBuffer_API_data;

NSP_GL_END

#pragma once

#include "GraphicsManager.hpp"
#include "GraphicsComponentBase.hpp"
#include "Shader.hpp"

NSP_GL_BEGIN
class CommandBuffer : GraphicsComponentBase {
public:
	friend class GraphicsManager;
	friend class CommandBuffer_API_data;
	struct Command {
		enum Type {
			BindVertexBuffer,
			BindIndexBuffer,
			SetShader,
			Draw,
		} cmd;

		struct Arg {
			Arg* next;
			virtual size_t getSize() const = 0;
			virtual void* getData() const = 0;

			virtual ~Arg() {};

		} *arg_chain;

		struct ArgIterator {
			Arg* current;

			ArgIterator(Arg* arg) {
				current = arg;
			}

			ArgIterator& operator++() {
				current = current->next;
				return *this;
			}

			Arg& operator*() {
				return *current;
			}

			ArgIterator operator++(int) { ArgIterator tmp = *this; ++(*this); return tmp; }
		};


		template<typename T>
		struct TypedArg : public Arg {
			T data;
			size_t getSize() const override { return sizeof(T); }
			void* getData() const override { return (void*)&data; }

			~TypedArg() {
				if(next) delete next;
			}
		};

        template<typename... ArgsT>
		struct __convert_args {
        	static Arg* result() { return nullptr; };
        };

        template<typename FirstT, typename... ArgsT>
		struct __convert_args<FirstT, ArgsT...> {
        	static Arg* result(FirstT first, ArgsT... args) {
        		TypedArg<FirstT>* t_arg = new TypedArg<FirstT>;
        		t_arg->data = first;
        		t_arg->next = __convert_args<ArgsT...>::result(args...);
        		return t_arg;
        	};
        };

		template<typename... ArgsT>
		Command(Type cmd, ArgsT... args) : cmd(cmd) {
			arg_chain = __convert_args<ArgsT...>::result(args...);
		}

		Command(const Command& other) = delete;

		~Command() {
			if(arg_chain) delete arg_chain;
		}
	};

private:
	pCommandBuffer_API_data api_data;

	vec<SmartPtr<Command>> commands;
public:
	CommandBuffer(pGraphicsManager g_mgr);
	~CommandBuffer() { cleanup(); }

	void reset();

	void drawVertices(uint count);
	void setActiveShader(pShader shader);

	void cleanup() override;

	void build();
};
NSP_GL_END
