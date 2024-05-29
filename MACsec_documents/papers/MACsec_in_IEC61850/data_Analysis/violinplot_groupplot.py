import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Parameters for file paths
GOOSE_PATHS = [
    ('duration_data/GOOSE_intervals/durations_GOOSE_no_encryption.csv', 'GOOSE\n(no encryption)'),
    ('duration_data/GOOSE_intervals/durations_GOOSE_AES_encryption.csv', 'GOOSE\n(MACsec encrypted)'),
    ('duration_data/GOOSE_intervals/durations_GOOSE_AES_integrity.csv', 'GOOSE\n(MACsec integrity)')
]

SV_PATHS = [
    ('duration_data/SV_intervals/durations_SV_no_encryption.csv', 'SV\n(no encryption)'),
    ('duration_data/SV_intervals/durations_SV_AES_encryption.csv', 'SV\n(MACsec encrypted)'),
    ('duration_data/SV_intervals/durations_SV_AES_integrity.csv', 'SV\n(MACsec integrity)')
]

MMS_PATHS = [
    ('duration_data/MMS_intervals/durations_MMS_no_encryption.csv', 'MMS\n(no encryption)'),
    ('duration_data/MMS_intervals/durations_MMS_AES_encryption.csv', 'MMS\n(MACsec encrypted)'),
    ('duration_data/MMS_intervals/durations_MMS_AES_integrity.csv', 'MMS\n(MACsec integrity)')
]

MMS2_PATHS = [
    ('duration_data/MMS_intervals/2_durations_MMS_no_encryption.csv', 'MMS\n(no encryption)'),
    ('duration_data/MMS_intervals/2_durations_MMS_AES_encryption.csv', 'MMS\n(MACsec encrypted)'),
    ('duration_data/MMS_intervals/2_durations_MMS_AES_integrity.csv', 'MMS\n(MACsec integrity)')
]

FILE_PATHS = MMS2_PATHS

# Read data from all CSV files
duration_data_all = []
boxplot_labels = []
medians = []

for file_path, label in FILE_PATHS:
    df = pd.read_csv(file_path, sep=',')
    duration_data = (df['Duration [s]'] * 1000000)      # Convert the timing data in us
    duration_data_all.append(duration_data)
    boxplot_labels.append(label)
    median_duration = np.median(duration_data)
    medians.append(median_duration)

# Create a new figure
plt.figure(figsize=(12, 12))

# Create the violin plot
violin_parts = plt.violinplot(duration_data_all, showmedians=True, widths=0.4)

# Set labels and title
plt.ylabel('Transmission Time [$\mu$s]', labelpad=20, fontsize=22)
#plt.xlabel('Measurement configuration', fontsize=22)
#plt.title('MMS Single Event Measurement', fontsize=16)

# Set names for each boxplot on the x-axis
plt.xticks(range(1, len(FILE_PATHS) + 1), boxplot_labels, fontsize=22)

# Increase fontsize of the numbers on both x and y axis
plt.tick_params(axis='both', which='major', labelsize=16)  # Increase font size of tick labels


# Print median values on the plot
for i, median in enumerate(medians):
    plt.text(i + 1.2, median, f'{median:.2f} $\mu$s', fontsize=14, color='black')

# Remove the top and right spines
plt.gca().spines['top'].set_visible(False)
plt.gca().spines['right'].set_visible(False)

# Save the plot as an image
plt.savefig('Group_ViolinPlot.png')

# Show the plot
plt.show()
