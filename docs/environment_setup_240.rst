.. _environment_setup_v240:

Requirements and setup for NCS V2.4.0
#####################################

This page outlines the requirements that you need to meet before you start working with the ESL environment.
Once completed, you will be able to run and test a sample ESL application.

Hardware requirements
*********************

To meet the hardware requirements, ensure you have at least two Nordic's boards, one for AP role and one for Tag role.

For Tag from the list of supported models:

  * `nRF52832 DK`_
  * `nRF52833 DK`_ (Recommended for first evaluation)
  * `nRF52840 DK`_
  * `nRF52840 Dongle`_
  * `Nordic Thingy:52`_

Optionally, you can use `WAVESHARE e-Paper Raw Panel Shield`_ and `WAVESHARE 250x122, 2.13inch E-Ink raw display panel`_ as a display device in combination with `nRF52833 DK`_ (refer to the :ref:`peripheral_esl_sample_activating_epd_variants`).

For Access Point from the list of supported models:

  * `nRF5340 DK`_ (Recommended for first evaluation)
  * `Nordic Thingy:53`_

Software requirements
*********************

To meet the software requirements, install the following tools:

  * `nRF Connect for Desktop`_
  * `nRF Command Line Tools`_
  * `GO Language`_
  * `Mcumgr Command-line Tool`_

Then:
   Install `nRF Connect SDK v2.4.0`_

nRF Connect SDK
===============

Perform the following steps to install `nRF Connect SDK v2.4.0`_ and setup boards:

#. Set up your development environment by choosing one of the installation methods below:

   * Follow `Installing automatically`_ guildelines to perform an automatic installation through the Toolchain Manager in the nRF Connect for Desktop or.
   * Follow `Installing manually`_ guidelines to perform a manual installation.

   .. note::
      For additional information on setting up the device as well as Nordic’s development environment and tools, see the `nRF Connect SDK Getting started guide`_.

#. Clone/Download the entire ESL repository.

   Use git:

   .. code-block:: console

      $ git clone https://github.com/NordicPlayground/nrf-esl-bluetooth -b v2.4-branch

   or Download the repository from `esl_archive_v2.4-branch`_ , and then unzip it:

   .. code-block:: console

      $ unzip main.zip

#. To build the samples using VS Code or Command line, follow this guide: `Building and programming an application`_.


.. _nRF Connect SDK v2.4.0: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.0/nrf/index.html
.. _Getting started with nRF52 Series: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/working_with_nrf/nrf52/gs.html
.. _Getting started with nRF53 Series: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/working_with_nrf/nrf53/nrf5340_gs.html
.. _Getting started with Thingy\:52: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/boards/arm/thingy52_nrf52832/doc/index.html
.. _Getting started with Thingy\:53: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/nrf/working_with_nrf/nrf53/thingy53_gs.html
.. _nRF Connect SDK Getting started guide: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.0/nrf/getting_started.html
.. _Installing automatically: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.0/nrf/gs_assistant.html#installing-automatically
.. _Installing manually: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.0/nrf/gs_installing.html#install-the-required-tools
.. _nRF Connect for Desktop: https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Connect-for-desktop
.. _GO Language: https://go.dev/doc/install
.. _Mcumgr Command-line Tool: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.0/zephyr/services/device_mgmt/mcumgr.html#command-line-tool
.. _esl_archive_v2.4-branch: https://github.com/NordicPlayground/nrf-esl-bluetooth/archive/refs/heads/v2.4-branch.zip
.. _Building and programming an application: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.0/nrf/getting_started/programming.html
