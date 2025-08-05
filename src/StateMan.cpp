#include <StateMan.h>


Engine::StateMan::StateMan() 
: m_add { false }, m_replace { false }, m_popCount { 0 }
{}

Engine::StateMan::~StateMan() 
{

}

void Engine::StateMan::Add(std::unique_ptr<Engine::State> toAdd, bool replace)
{
    this->m_add = true;
    this->m_newState = std::move(toAdd);

    m_replace = replace;
}


void Engine::StateMan::PopCurrent()
{
    ++m_popCount;
}

void Engine::StateMan::PopMultiple(int n)
{
    m_popCount += n;
}

void Engine::StateMan::ProcessStateChanged()
{
    while (m_popCount > 0 && !m_stateStack.empty())
    {
        m_stateStack.pop();
        --m_popCount;
    }

    // After poping resume new state
    if (!m_stateStack.empty())
        m_stateStack.top()->Start();

    // Pause current state and add new state
    if (m_add)
    {
        if (m_replace && !m_stateStack.empty())
        {
            m_stateStack.pop();
            m_replace = false;
        }

        if (!m_stateStack.empty())
            m_stateStack.top()->Pause();

        m_stateStack.push(std::move(m_newState));
        m_stateStack.top()->Init();
        m_stateStack.top()->Start();

        m_add = false;
    }
}



std::unique_ptr<Engine::State>& Engine::StateMan::GetCurrent()
{
    return m_stateStack.top();
}

