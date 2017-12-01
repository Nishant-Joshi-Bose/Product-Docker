Git Checkout
cd /scratch
git clone git@github.com:BoseCorp/CastleTools.git
PATH=$PATH:/scratch/CastleTools/bin
git clone git@github.com:BoseCorp/Eddie.git
cd Eddie

Install python modules
sudo pip install -r Test/demoController/requirements.py

Run tests
cd Test
pytest demoController/test_demo.py -vs --target=device --network-iface='eth0' --device-id=61b5457