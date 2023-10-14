import subprocess
import matplotlib.pyplot as plt
import time as t

interface_name = "wlp2s0"

values = []
times = []
time = 0

command = f"ip -s link show {interface_name}"
prev_value = 0
for i in range(0, 30):
    try:
        output = subprocess.check_output(command, shell=True, universal_newlines=True)
        lines = output.split('\n')
        rx_line = lines[3]
        tx_line = lines[5]
        rx_value = None
        tx_value = None
        if rx_line:
            rx_value = rx_line.split()[0]

        if tx_line:
            tx_value = tx_line.split()[0]
        if prev_value == 0:
            prev_value = int(rx_value) + int(tx_value)

        times.append(time)
        time += 1

        values.append((int(rx_value) + int(tx_value)) - prev_value)
        prev_value = int(rx_value) + int(tx_value)
        t.sleep(1)
    except subprocess.CalledProcessError as e:
        print(f"Ошибка выполнения команды: {e}")
    except Exception as e:
        print(f"Произошла ошибка: {e}")
fig, ax = plt.subplots()
ax.plot(times, values)
plt.savefig("net_ip.png")

