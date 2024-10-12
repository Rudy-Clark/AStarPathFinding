#include "Commands.hpp"

Commands::Commands()
{
	ClearCommands();
}

void Commands::Push(Vector2 coord)
{
	for (size_t i{m_size + 1}; i > 0; --i) {
		m_commands[i] = m_commands[i - 1];
	}
	m_size = (m_size + 1) % m_SIZE;
	m_commands[0] = coord;
}

void Commands::ClearCommands()
{
	for (int i{ 0 }; i < m_SIZE; ++i) {
		m_commands[i] = { -1.f, -1.f };
	}
}

int Commands::GetSize() const
{
	int count{ 0 };
	for (int i{ 0 }; i < m_SIZE; ++i) {
		if (m_commands[i].x >= 0 && m_commands[i].y >= 0) {
			++count;
		}
	}

	return count;
}
