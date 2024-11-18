import serial
import numpy as np
import matplotlib.pyplot as plt

# Cấu hình cổng serial (COM22, baudrate: 115200, với timeout)
ser = serial.Serial('COM22', 115200, timeout=1)

# Giới hạn nhiệt độ tối thiểu và tối đa
min_temp = 25
max_temp = 120

def read_serial_data():
    """
    Đọc dữ liệu nhiệt độ từ cổng serial.
    Giả sử dữ liệu gồm 64 giá trị float ngăn cách bởi dấu phẩy.
    """
    try:
        # Đọc một dòng dữ liệu từ cổng serial
        ser_data = ser.readline().decode('utf-8').strip()

        # Chuyển đổi chuỗi dữ liệu thành danh sách các giá trị float
        temp_values = list(map(float, ser_data.split(',')))
        if len(temp_values) == 64:
            return np.array(temp_values).reshape(8, 8)
        else:
            print(f"Định dạng dữ liệu không hợp lệ: {ser_data}")
            return None
    except Exception as e:
        print(f"Lỗi đọc dữ liệu từ serial: {e}")
        return None

def update_heatmaps(data_matrix, im1, im2):
    """
    Cập nhật dữ liệu trên biểu đồ nhiệt.
    """
    global min_temp, max_temp  # Khai báo các biến toàn cục để cập nhật giá trị

    # Cập nhật giới hạn nhiệt độ dựa trên dữ liệu nhận được
    max_temp = np.max(data_matrix)
    min_temp = np.min(data_matrix)    
    # max_temp = 150
    # min_temp = 25

    # Cập nhật dữ liệu cho các biểu đồ nhiệt
    im1.set_data(data_matrix)
    im2.set_data(data_matrix)
    
    # Cập nhật giới hạn nhiệt độ trên biểu đồ
    im1.set_clim(vmin=min_temp, vmax=max_temp)
    im2.set_clim(vmin=min_temp, vmax=max_temp)

    # Vẽ lại biểu đồ
    plt.draw()
    plt.pause(0.001)

def plot_heatmaps():
    """
    Tạo hai biểu đồ nhiệt - dạng pixel và dạng mượt mà.
    """
    # Khởi tạo ma trận dữ liệu nhiệt độ 8x8 ban đầu với giá trị 0
    data_matrix = np.zeros((8, 8))

    # Tạo các đồ thị
    fig, axes = plt.subplots(1, 2, figsize=(12, 6))

    # Biểu đồ nhiệt dạng pixel
    im1 = axes[0].imshow(data_matrix, cmap='coolwarm', interpolation='nearest', vmin=min_temp, vmax=max_temp)
    axes[0].set_title("Biểu đồ nhiệt dạng pixel")
    axes[0].grid(False)

    # Biểu đồ nhiệt mượt mà
    im2 = axes[1].imshow(data_matrix, cmap='coolwarm', interpolation='bilinear', vmin=min_temp, vmax=max_temp)
    axes[1].set_title("Biểu đồ nhiệt mượt mà")
    axes[1].grid(False)

    # Thanh màu cho mỗi biểu đồ
    fig.colorbar(im1, ax=axes[0], label='Nhiệt độ (°C)', extend='both')
    fig.colorbar(im2, ax=axes[1], label='Nhiệt độ (°C)', extend='both')

    plt.tight_layout()
    plt.ion()
    plt.show()

    return im1, im2

if __name__ == "__main__":
    # Khởi tạo hai biểu đồ nhiệt và các ảnh
    im1, im2 = plot_heatmaps()

    while True:
        # Đọc dữ liệu từ cổng serial
        temp_matrix = read_serial_data()

        # Nếu dữ liệu hợp lệ, cập nhật biểu đồ nhiệt
        if temp_matrix is not None:
            print("Dữ liệu nhiệt độ nhận được:")
            print(temp_matrix)
            update_heatmaps(temp_matrix, im1, im2)
