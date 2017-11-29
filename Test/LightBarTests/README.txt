command to perform the lightbar animation:--

(1)
For getting the animation value that is playing.

pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_getActiveAnimation --ip-address=<ip-address> --LPM_port=<lpm_port>

(2)
For delete the animation playing using the following command:-

pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_deleteActiveAnimation --ip-address=<ip-address> --LPM_port=<lpm_port>

(3)
To run the negative test scenarios for lightbar animation:-

pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_playInValidAnimationScenarios --ip-address=<ip-address> --LPM_port=<lpm_port>


(4)
To run the positive test scenarios for lightbar animation:-

pytest -vs test_localui_lightbar_script.py::Test_lightbar::test_playValidAnimationScenarios --ip-address=<ip-address> --LPM_port=<lpm_port>


