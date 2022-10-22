import requests
import time

count=0
while count< 1000:
  print(count)
  requests.get('http://192.168.4.101/relay/0') #ON
  time.sleep(3)
  requests.get('http://192.168.4.101/relay/1') #OFF
  time.sleep(3)
  count=count+1

