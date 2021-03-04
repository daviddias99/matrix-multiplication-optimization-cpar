from matplotlib import pyplot as plt
import pandas as pd
from os import path

data = pd.read_csv(path.join(path.dirname(__file__), 'results/exp_1_2021-03-03 09:03:20.271109.csv'))
data_2 = data.groupby(['Language','Matrix Size'],  as_index=False).mean()
data_2.reset_index()
ax = data_2[['Language','Matrix Size', 'Time']][data_2['Language'] == 'cpp'].plot(x='Matrix Size',y='Time', label='Cpp', kind='scatter', color='red')
data_2[['Language','Matrix Size', 'Time']][data_2['Language'] == 'java'].plot(x='Matrix Size',y='Time', ax=ax, label='Java', kind='scatter', style=['ro'])
plt.show()
