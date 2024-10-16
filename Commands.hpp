#pragma once
#include "raylib.h"
#include <array>

class Commands
{
	public:
		Commands();

		void Push(Vector2 coord);

		inline const Vector2& GetStart() const { return GetSize() == 1 ? m_commands[0] : m_commands[m_SIZE - 1]; };
		inline const Vector2& GetEnd() const { return GetSize() == 1 ? m_commands[m_SIZE - 1] : m_commands[0]; };
		void ClearCommands();
		int GetSize() const;


private:
	// by now accept only 2 commands
	static constexpr size_t m_SIZE{ 2 };
	// Just STACK!!!
	Vector2 m_commands[m_SIZE];
	size_t m_size{};
};

