This project is aimed to act as gateway between Felicity Solar / Felicity ESS battery management systems connected via RS485 and MQTT server.

Technically the RS485 task runs in 5 second intervals pushing data into a queue while the MQTT task listens for incoming packets.

Data is forwarded to configured MQTT server

<img width="355" height="796" alt="image" src="https://github.com/user-attachments/assets/81125f4b-9d4d-46c7-bd27-6f95b43bc825" />

