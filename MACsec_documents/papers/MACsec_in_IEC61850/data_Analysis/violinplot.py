import os
import pandas as pd
import matplotlib.pyplot as plt

# Source
#FILE_PATH = 'raw_data/GOOSE_no_encryption.csv'
#FILE_PATH = 'raw_data/GOOSE_AES_encrypted.csv'
#FILE_PATH = 'raw_data/GOOSE_AES_integrity.csv'

#FILE_PATH = 'raw_data/SV_no_encryption.csv'
#FILE_PATH = 'raw_data/SV_AES_encrypted.csv'
FILE_PATH = 'raw_data/SV_AES_integrity.csv'

# Function to calculate time periods where Channel 1 and Channel 2 do not have the same value
def calculate_timediv_periods():
    # Read the data from the CSV file
    df = pd.read_csv(FILE_PATH)

    times = df["Time [s]"]
    ch1_values = df["Channel 1"]
    ch2_values = df["Channel 2"]

    periods = []
    timediv_start = None

    for i in range(len(times)):
        if ch1_values[i] == ch2_values[i]:
            if timediv_start is None:
                timediv_start = times[i]
        else:
            if timediv_start is not None:
                periods.append((timediv_start, times[i], times[i] - timediv_start))
                timediv_start = None

    # If the mismatch extends to the end of the data, close the period
    if timediv_start is not None:
        periods.append((timediv_start, times.iloc[-1], times.iloc[-1] - timediv_start))
    
    return periods



# Start of Application
if __name__ == '__main__':
    # Calculate time div periods
    timediv_periods = calculate_timediv_periods()
    
    # Print the results
    print("Periods where Channel 1 and Channel 2 do not have the same value:")
    for start, end, duration in timediv_periods:
        print(f"Start: {start:.9f}, End: {end:.9f}, Duration: {duration:.9f}")

    # Save the mismatch periods to a CSV file
    timediv_df = pd.DataFrame(timediv_periods, columns=["Start Time [s]", "End Time [s]", "Duration [s]"])
    timediv_df.to_csv('timediv_periods.csv', index=False)

    # Load your CSV file into a DataFrame
    df = pd.read_csv('timediv_periods.csv')

    # Convert 'Duration [s]' to microseconds
    df['Duration [us]'] = df['Duration [s]'] * 1000000

    # Extract the 'Duration [us]' column
    durations = df['Duration [us]']

    # Calculate the mean duration
    mean_duration_us = durations.mean()

    # Create a violin plot
    plt.violinplot(durations, showmeans=False, showmedians=True)

    # Customize the plot
    plt.xticks([1], ['GOOSE no encryption'])
    plt.ylabel(r'Time [$\mu$s]')
    plt.title('Violin Plot of Durations')
    plt.text(1.05, mean_duration_us, f'Median: {mean_duration_us:.2f} us', fontsize=12, color='blue')
    
    # Display the plot and save
    plt.savefig('plots/violinPlot.png')
    plt.show()
    
    