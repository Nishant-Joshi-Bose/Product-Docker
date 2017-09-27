.PHONY: all
all:
	rm -rf CastleTestUtils
	git clone git@github.com:BoseCorp/CastleTestUtils
	cp CastleTestUtils/SoftwareUpdateUtils/LpmUtils/lpm_updater.py .
	chmod +x lpm_updater.py
	rm -rf CastleTestUtils
	sudo pyinstaller --onefile  eddie_flash.py fastboot_flash.py lpm_updater.py
	rm lpm_updater.py
	cp dist/eddie_flash ../lin/product_flash
	
.PHONY: clean
clean:
	sudo rm -rf CastleTestUtils dist build *.pyc *.log *.spec lpm_updater.py
