/*
 * CommandBuffer.hpp
 *
 *  Created on: 26 April 2022
 *      Author: FileBinsLapTop
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

class CommandBuffer;
typedef RefPtr<CommandBuffer> pCommandBuffer;

struct CommandBuffer_API_data;
typedef RefPtr<CommandBuffer_API_data> pCommandBuffer_API_data;

NSP_GL_END

#pragma once

#include "GraphicsManager.hpp"
#include "GraphicsComponentBase.hpp"
#include "Shader.hpp"
#include "Buffer.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "UniformBuffer.hpp"

NSP_GL_BEGIN
class CommandBuffer : GraphicsComponentBase {
public:
	friend class GraphicsManager;
	friend struct CommandBuffer_API_data;
	better_class(Command) {
	public:
		enum Type {
			BindVertexBuffer,
			BindIndexBuffer,
			SetShader,
			Draw,
			DrawIndexed,
			PushConstants,
			BindUniformBuffer,
		} cmd;

		struct Arg {
			Arg* next = 0;
			virtual size_t getSize() const = 0;
			virtual void* getData() const = 0;
			virtual Arg* clone() const = 0;
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

			TypedArg() = default;
			TypedArg(const T& d) : data(d) {};

			Arg* clone() const override {
				TypedArg<T>* arg = new TypedArg<T>(data);
				if(next) arg->next = next->clone();
				return arg;
			}
			~TypedArg() {
				if(next) delete next;
			}
		};

	private:

        template<typename... ArgsT>
		struct __convert_args {
        	static Arg* result() { return nullptr; };
        };

        template<typename FirstT, typename... ArgsT>
		struct __convert_args<FirstT, ArgsT...> {
        	static Arg* result(FirstT first, ArgsT... args) {
        		TypedArg<FirstT>* t_arg = new TypedArg<FirstT>(first);
        		t_arg->next = __convert_args<ArgsT...>::result(args...);
        		return t_arg;
        	};
        };

	public:
		template<typename... ArgsT>
		Command(Type cmd, ArgsT... args) : cmd(cmd) {
			arg_chain = __convert_args<ArgsT...>::result(args...);
		}

		Command(const Command& other) {
			cmd = other.cmd;
			arg_chain = other.arg_chain->clone();
		}

		~Command() {
			if(arg_chain) delete arg_chain;
		}
	};

protected:
	EventRegistration resizeReg;
	AutoPtr<CommandBuffer_API_data> api_data;

	std::vector<AutoPtr<Command>> commands;
public:
	CommandBuffer(pGraphicsManager g_mgr);
	~CommandBuffer() { cleanup(); }

	void clear();

	void drawVertices(uint count);
	void drawIndexed(uint index_count);
	void setActiveShader(pShader shader);

	void bindVertexBuffer(pBuffer buffer);
	void bindVertexBuffer(pVertexBuffer vertexBuffer);
	void bindIndexBuffer(pIndexBuffer indexBuffer);
	void bindUniformBuffer(pUniformBuffer uniformBuffer);

	template<typename T>
	void pushConstants(T data, pShader shader, Shader::Type stage = Shader::Vertex) {
		commands.push_back(new_ref<Command>(Command::PushConstants, shader, stage, (uint)sizeof(data), data));
	}

	void cleanup() override;

	void build();
	void buildAll();
};
NSP_GL_END
