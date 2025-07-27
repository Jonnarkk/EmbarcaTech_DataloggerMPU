import pandas as pd
import matplotlib.pyplot as plt

# Nome do arquivo CSV gerado 
filename = 'logs_mpu.csv'

try:
    # Carrega os dados usando a biblioteca Pandas
    #    - `pd.read_csv` lê o arquivo CSV.
    #    - `sep=','` informa que as colunas são separadas por vírgula.
    #    - `parse_dates=['Timestamp']` diz ao Pandas para tratar a primeira coluna como data/hora.
    #    - `index_col='Timestamp'` usa a coluna de data/hora como o índice do nosso gráfico (eixo X).
    data = pd.read_csv(filename, sep=',', parse_dates=['Timestamp'], index_col='Timestamp')

    # Cria a figura e os subplots
    #    - `plt.subplots(3, 1, ...)` cria uma figura com 3 gráficos, um abaixo do outro.
    #    - `figsize=(10, 12)` define o tamanho da janela da figura.
    #    - `sharex=True` faz com que todos os gráficos compartilhem o mesmo eixo X (tempo).
    fig, axs = plt.subplots(3, 1, figsize=(10, 12), sharex=True)
    fig.suptitle('Dados do Sensor MPU6050', fontsize=16)

    # Plota os dados de Aceleração no primeiro gráfico
    axs[0].plot(data.index, data['AccX'], label='Ace. em X', color='r')
    axs[0].plot(data.index, data['AccY'], label='Ace. em Y', color='g')
    axs[0].plot(data.index, data['AccZ'], label='Ace. em Z', color='b')
    axs[0].set_title('Acelerômetro')
    axs[0].set_ylabel('Aceleração (g)')
    axs[0].legend()
    axs[0].grid(True)

    # Plota os dados do Giroscópio no segundo gráfico 
    axs[1].plot(data.index, data['Roll'], label='Ângulo do Roll', color='r')
    axs[1].plot(data.index, data['Pitch'], label='Ângulo do Pitch', color='g')
    axs[1].set_title('Giroscópio')
    axs[1].set_ylabel('Ângulo (Graus)')
    axs[1].legend()
    axs[1].grid(True)

    # Plota os dados de Temperatura no terceiro gráfico 
    axs[2].plot(data.index, data['TempC'], label='Temperatura', color='m')
    axs[2].set_title('Temperatura do Chip')
    axs[2].set_ylabel('Graus Celsius (°C)')
    axs[2].set_xlabel('Tempo (Iterações a cada 100 ms)')
    axs[2].legend()
    axs[2].grid(True)

    # Ajusta o layout para evitar sobreposição de títulos
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    
    # Exibe a janela com os gráficos
    plt.show()
    # Possíveis erros encontrados
except FileNotFoundError:
    print(f"Erro: O arquivo '{filename}' nao foi encontrado.")
    print("Certifique-se de que o script Python esta na mesma pasta que o arquivo CSV.")
except Exception as error:
    print(f"Ocorreu um erro ao processar o arquivo: {error}")

