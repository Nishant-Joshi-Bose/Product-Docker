.PHONY: all
all:
	pyinstaller --onefile  eddie_flash.py fastboot_flash.py adb_communication.py
	cp dist/eddie_flash ../lin/product_flash
	
.PHONY: clean
clean:
	rm -rf CastleTestUtils dist build *.pyc *.log *.spec
