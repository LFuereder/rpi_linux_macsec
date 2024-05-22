import pandas as pd

# Read the data from a CSV file
df = pd.read_csv('raw_data/MMS_AES_encrypted.csv')

# Initialize a list to hold the periods
periods = []

# Iterate through the DataFrame to calculate durations
for i in range(1, len(df)):
    if df.loc[i-1, 'Channel 2'] == 1 and df.loc[i, 'Channel 2'] == 0:
        start_time = df.loc[i-1, 'Time [s]']
        end_time = df.loc[i, 'Time [s]']
        duration = (end_time - start_time) / 2
        periods.append((start_time, end_time, duration))

# Create a DataFrame from the periods list
periods_df = pd.DataFrame(periods, columns=['Start Time [s]', 'End Time [s]', 'Duration [s]'])

# Print resulting periods
print(periods_df)

# Write the periods DataFrame to a new CSV file
periods_df.to_csv('periods.csv', index=False)

print("Periods have been written to periods.csv")
