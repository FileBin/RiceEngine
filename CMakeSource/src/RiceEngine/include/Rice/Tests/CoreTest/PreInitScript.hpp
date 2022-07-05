#include "../../stdafx.hpp"

#include "../../Engine/Core.hpp"

NSP_TESTS_BEGIN

class PreInitScript : public ScriptBase {
    void run() override {
        pEngine en = getEngine();
        //scriptRegistartion
        en->registerScript(new_ref<InitScript>(), Stage::Init);
    }
}

NSP_TESTS_END