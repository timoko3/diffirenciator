import numpy as np
import matplotlib.pyplot as plt

# --------------------------------------------------------
# 1. ВПИСЫВАЕШЬ ВРУЧНУЮ ВСЕ ВЫРАЖЕНИЯ
# --------------------------------------------------------

def f(x):          # исходная функция
    return x**2

def taylor(x):     # разложение Тейлора (вписано вручную!)
    return x**2

def f_prime(x):    # производная функции
    return 2*x

def taylor_prime(x):  # производная разложения
    return 2*x


# --------------------------------------------------------
# 2. ГЕНЕРАЦИЯ ТОЧЕК
# --------------------------------------------------------

x_vals = np.linspace(-5, 5, 400)

# --------------------------------------------------------
# 3. ПОСТРОЕНИЕ ГРАФИКОВ
# --------------------------------------------------------

plt.figure(figsize=(10, 6))

plt.plot(x_vals, f(x_vals), label="f(x) = x²", linewidth=2)
plt.plot(x_vals, taylor(x_vals), '--', label="Тейлор f(x)", linewidth=2)
plt.plot(x_vals, f_prime(x_vals), label="f'(x) = 2x", linewidth=2)
plt.plot(x_vals, taylor_prime(x_vals), '--', label="Тейлор f'(x)", linewidth=2)

# --------------------------------------------------------
# 4. ОФОРМЛЕНИЕ
# --------------------------------------------------------

plt.title("Функция, её Тейлор, производная и производная Тейлора", fontsize=14)
plt.xlabel("x", fontsize=12)
plt.ylabel("y", fontsize=12)
plt.grid(True, alpha=0.3)
plt.legend(fontsize=12)
plt.tight_layout()

plt.savefig("graph.jpg")
plt.show()
