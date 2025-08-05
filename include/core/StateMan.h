#include <stack>
#include <memory>

#include "Config.h"
#include "State.h"

namespace Engine 
{
    class StateMan
    {
        private:
            bool m_add;
            bool m_replace;
            int m_popCount = 0;

            std::stack<std::unique_ptr<State>> m_stateStack;
            std::unique_ptr<State> m_newState;
        public:
            StateMan();
            ~StateMan();

            void Add(std::unique_ptr<State> toAdd, bool replace = false);
            void PopCurrent();
            void PopMultiple(int n);
            void ProcessStateChanged();
            std::unique_ptr<Engine::State>& GetCurrent();
    };  
};