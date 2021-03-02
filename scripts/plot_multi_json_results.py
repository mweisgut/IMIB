#!/usr/bin/env python3

from json_utils import json_to_dataframe

from matplotlib.cm import get_cmap
import glob
import json
import matplotlib.pyplot as plt
import matplotlib.ticker as tkr
import numpy as np
import pandas as pd
import seaborn as sns
import sys

old_name_replacements = {
    "Art": "Unsync ART",
    "ART": "Unsync ART",
    "BTreeGoogleAbseil": "Abseil B-Tree",
    "BTreeTlx": "TLX B+ Tree",
    "B+ Tree TLX": "TLX B+ Tree",
    "HashMapRobinHoodFlat": "RH Flat Map",
    "HashMapRobinHoodNode": "RH Node Map",
    "HashMapTslRobin": "TSL Robin Map",
    "HashMapTslSparse": "TSL Sparse Map",
    "Judy": "MP Judy",
    "SimpleVector": "Simple Vector",
    "SkipList": "PG Skip List",
    "SortedVector": "Sorted Vector",
    "UnorderedMap": "STD Hash Map",
}

# tuples: name, line style
NUM_REF_COMPETITORS = 2  # simple vector and sorted vector
competitors = [
    ("Unsync ART", "x-"),
    ("Abseil B-Tree", "+-"),
    ("TLX B+ Tree", "o-"),
    ("RH Flat Map", "^-"),
    ("RH Node Map", "v-"),
    ("TSL Robin Map", "D-"),
    ("TSL Sparse Map", "d-"),
    ("MP Judy", ".-"),
    ("STD Hash Map", "s-"),
    ("BB-Tree", "p-"),  # early excluded
    ("PG Skip List", "P-"),  # early excluded
    ("Simple Vector", ":"),  # reference value
    ("Sorted Vector", "--"),  # reference value
]

# columns
c_case_name = "cases.case_name"
c_duration = "cases.executions.duration_ns"
c_executions = "cases.executions"
c_index_name = "cases.index_name"
c_index_size = "cases.executions.index_size_bytes"
c_iter = "configuration.iterations"
c_key_type = "cases.key_type"
c_data_size = "cases.data_size"
c_threads = "configuration.threads"
c_val_type = "cases.value_type"

SUBPLOT_COL_SIZE = 4
SUBPLOT_ROW_SIZE = 4
SUBPLOT_HSPACE = 0.12
SUBPLOT_WSPACE = 0.05
SUBPLOT_FILE_PREFIX = "Subplots"
LEGEND_COLUMNS = 4
LEGEND_POS_Y = 0.038
MARKER_SIZE = 5


def json_to_filtered_median_dataframe(jdata):
    df = json_to_dataframe(jdata)
    df = df.drop(columns=[c_executions, c_iter, c_threads])
    df.dropna(inplace=True)
    df.reset_index(drop=True, inplace=True)
    df_med = df.groupby([c_case_name, c_index_name, c_key_type, c_val_type], as_index=False).median()
    df_med.set_index(c_case_name, inplace=True)
    df_med.replace(0, np.nan, inplace=True)
    df_med[c_index_name] = df_med[c_index_name].replace(old_name_replacements)
    return df_med


def generate_style_for_competitors(competitor_names):
    style = []
    for included_comp_name in competitor_names:
        for comp_name, comp_style in competitors:
            if comp_name == included_comp_name:
                style.append(comp_style)
                break

    return style


def competitor_color_mapping():
    color_mapping = {}
    colors = [c for c in get_cmap("tab20").colors]
    colors = sns.cubehelix_palette(
        n_colors=len(competitors) - NUM_REF_COMPETITORS, start=0, rot=4, hue=1.8, dark=0.1, light=0.8
    )
    for i in range(NUM_REF_COMPETITORS):
        colors.append("darkgrey")
    for idx, comp in enumerate(competitors):
        comp_name, _ = comp
        color_mapping[comp_name] = colors[idx]

    return color_mapping


def create_df():
    file_paths = sorted(glob.glob("*json"))
    print("Files relevant for creating the plots:")
    for path in file_paths:
        print("  " + path)

    data_frames = []

    # file example: dense_shuffled_uint64_uint32_000200000_2021-02-06_17-12-40.json
    for file_path in file_paths:
        with open(file_path) as json_file:
            jdata = json.load(json_file)
            df = json_to_filtered_median_dataframe(jdata)
            file_fragments = file_path.split("_")
            df["data_distribution"] = file_fragments[0]
            df["data_ordering"] = file_fragments[1]
            # file_desc = "_".join(file_path.split("_")[:4])
            data_frames.append(df)

    df = pd.concat(data_frames)
    df.set_index(["data_distribution", "data_ordering"], inplace=True, append=True)
    df.sort_index(inplace=True)
    return df


def sub_plot_durations(df):
    color_mapping = competitor_color_mapping()
    cases = df.index.get_level_values(0).unique()
    distributions = df.index.get_level_values(1).unique()
    orderings = df.index.get_level_values(2).unique()

    nrows = len(distributions)
    ncols = len(orderings)

    ordering_names = {"asc": "ascending order", "shuffled": "random order"}

    shared_xlabel = None
    shared_ylabel = None
    for case in cases:
        fig, axes = plt.subplots(
            nrows=nrows,
            ncols=ncols,
            figsize=(ncols * SUBPLOT_COL_SIZE, nrows * SUBPLOT_ROW_SIZE),
            sharex=True,
            sharey=True,
        )
        for dist_idx, distribution in enumerate(distributions):
            for order_idx, ordering in enumerate(orderings):
                ax = axes[dist_idx, order_idx]
                sub_df = df.loc[(case, distribution, ordering)]

                # Filter competitors
                # if case == "Insert":
                #     sub_df = sub_df.loc[sub_df[c_index_name].isin(["BB-Tree", "Skip List", "B+ Tree TLX"])]
                # elif case == "EqualityLookup":
                #     sub_df = sub_df.loc[sub_df[c_index_name].isin(["BB-Tree", "Skip List", "Sorted Vector"])]

                plot_df = sub_df.pivot(index=c_data_size, columns=c_index_name, values=[c_duration, c_index_size])

                style = generate_style_for_competitors(plot_df.columns.get_level_values(1).unique())
                ax = plot_df.plot.line(
                    y=c_duration,
                    legend=False,
                    color=color_mapping,
                    style=style,
                    fillstyle="none",
                    markersize=MARKER_SIZE,
                    ax=ax,
                )
                ax.yaxis.grid(linewidth=0.5)
                ax.set_title(distribution.capitalize() + " data in " + ordering_names[ordering])

                ax.set_xlabel("")
                ax.get_xaxis().set_major_formatter(tkr.FuncFormatter(lambda x, p: "{0:.0f}".format(x * 10 ** (-6))))
                ax.get_xaxis().set_major_locator(tkr.IndexLocator(base=2000000, offset=0))
                # ax.get_xaxis().set_major_locator(tkr.MaxNLocator(min_n_ticks=5))
                shared_xlabel = "Number of index entries in million"

                ax.set_ylabel("")
                if case == "EqualityLookup":
                    format_str = "{0:.1f}"
                else:
                    format_str = "{0:.0f}"

                ax.get_yaxis().set_major_formatter(tkr.FuncFormatter(lambda y, p: format_str.format(y * 10 ** (-9))))
                shared_ylabel = "Duration in seconds"

        handles = None
        labels = None
        for ax in fig.axes:
            if labels is None:
                handles, labels = ax.get_legend_handles_labels()
            else:
                ax_handles, ax_labels = ax.get_legend_handles_labels()
                assert labels == ax_labels
                for idx, ax_handle in enumerate(ax_handles):
                    assert ax_handle.get_marker() == handles[idx].get_marker()
                    assert ax_handle.get_color() == handles[idx].get_color()

        # mode="expand", ncol=3
        left_x0 = axes[0, 0].get_position().x0
        right_x1 = axes[0, ncols - 1].get_position().x1
        center_x = left_x0 + (0.5 * (right_x1 - left_x0))
        fig.legend(
            handles,
            labels,
            loc="upper center",
            borderaxespad=0.0,
            bbox_to_anchor=(center_x, LEGEND_POS_Y),
            ncol=LEGEND_COLUMNS,
        )

        # one common x and y label
        fig.add_subplot(111, frameon=False)
        # hide ticks and tick labels of the new axis
        plt.tick_params(labelcolor="none", top=False, bottom=False, left=False, right=False)
        plt.xlabel(shared_xlabel, labelpad=10)
        plt.ylabel(shared_ylabel, labelpad=10)
        plt.subplots_adjust(hspace=SUBPLOT_HSPACE, wspace=SUBPLOT_WSPACE)
        # using tight layout results in positioning issues for the legend
        data_description_attributes = sub_df[[c_key_type, c_val_type]].iloc[0].values
        filename = (
            "../" + SUBPLOT_FILE_PREFIX + "_" + case + "_duration_" + "_".join(data_description_attributes) + ".pdf"
        )
        plt.savefig(filename, bbox_inches="tight")
        print("File saved: " + filename)
        plt.close(ax.get_figure())


def sub_plot_memory(df):
    color_mapping = competitor_color_mapping()
    cases = df.index.get_level_values(0).unique()
    distributions = df.index.get_level_values(1).unique()
    orderings = df.index.get_level_values(2).unique()

    nrows = len(distributions)
    ncols = len(orderings)

    ordering_names = {"asc": "ascending order", "shuffled": "random order"}

    shared_xlabel = None
    shared_ylabel = None

    for case in cases:
        fig, axes = plt.subplots(
            nrows=nrows,
            ncols=ncols,
            figsize=(ncols * SUBPLOT_COL_SIZE, nrows * SUBPLOT_ROW_SIZE),
            sharex=True,
            sharey=True,
        )
        plot_created = False
        for dist_idx, distribution in enumerate(distributions):
            for order_idx, ordering in enumerate(orderings):
                ax = axes[dist_idx, order_idx]
                sub_df = df.loc[(case, distribution, ordering)]

                # Filter competitors
                # if case == "Insert":
                #     sub_df = sub_df.loc[sub_df[c_index_name].isin(["BB-Tree", "Skip List", "B+ Tree TLX"])]
                # elif case == "EqualityLookup":
                #     sub_df = sub_df.loc[sub_df[c_index_name].isin(["BB-Tree", "Skip List", "Sorted Vector"])]

                plot_df = sub_df.pivot(index=c_data_size, columns=c_index_name, values=[c_duration, c_index_size])
                if plot_df[c_index_size].isnull().values.all():
                    continue

                drop_columns = [c_duration]
                for drop_comp in ["Simple Vector", "Sorted Vector"]:
                    if (c_index_size, drop_comp) in plot_df.columns:
                        drop_columns.append((c_index_size, drop_comp))

                plot_df = plot_df.drop(columns=drop_columns)
                plot_created = True
                style = generate_style_for_competitors(plot_df.columns.get_level_values(1).unique())
                ax = plot_df.plot.line(
                    y=c_index_size,
                    legend=False,
                    color=color_mapping,
                    style=style,
                    fillstyle="none",
                    markersize=MARKER_SIZE,
                    ax=ax,
                )
                ax.yaxis.grid(linewidth=0.5)
                ax.set_title(distribution.capitalize() + " data in " + ordering_names[ordering])

                ax.set_xlabel("")
                ax.get_xaxis().set_major_formatter(tkr.FuncFormatter(lambda x, p: "{0:.0f}".format(x * 10 ** (-6))))
                ax.get_xaxis().set_major_locator(tkr.IndexLocator(base=2000000, offset=0))
                # ax.get_xaxis().set_major_locator(tkr.MaxNLocator(min_n_ticks=5))
                shared_xlabel = "Number of index entries in million"

                ax.set_ylabel("")
                ax.get_yaxis().set_major_formatter(tkr.FuncFormatter(lambda y, p: "{0:.0f}".format(y * 10 ** (-6))))
                shared_ylabel = "Index size in Megabytes"

        if not plot_created:
            continue

        handles = None
        labels = None
        for ax in fig.axes:
            if labels is None:
                handles, labels = ax.get_legend_handles_labels()
            else:
                ax_handles, ax_labels = ax.get_legend_handles_labels()
                assert labels == ax_labels
                for idx, ax_handle in enumerate(ax_handles):
                    assert ax_handle.get_marker() == handles[idx].get_marker()
                    assert ax_handle.get_color() == handles[idx].get_color()

        # mode="expand", ncol=3
        left_x0 = axes[0, 0].get_position().x0
        right_x1 = axes[0, ncols - 1].get_position().x1
        center_x = left_x0 + (0.5 * (right_x1 - left_x0))
        fig.legend(
            handles,
            labels,
            loc="upper center",
            borderaxespad=0.0,
            bbox_to_anchor=(center_x, LEGEND_POS_Y),
            ncol=LEGEND_COLUMNS,
        )

        # one common x and y label
        fig.add_subplot(111, frameon=False)
        # hide ticks and tick labels of the new axis
        plt.tick_params(labelcolor="none", top=False, bottom=False, left=False, right=False)
        plt.xlabel(shared_xlabel, labelpad=10)
        plt.ylabel(shared_ylabel, labelpad=10)
        plt.subplots_adjust(hspace=SUBPLOT_HSPACE, wspace=SUBPLOT_WSPACE)
        # using tight layout results in positioning issues for the legend
        data_description_attributes = sub_df[[c_key_type, c_val_type]].iloc[0].values
        filename = (
            "../" + SUBPLOT_FILE_PREFIX + "_" + case + "_index_size_" + "_".join(data_description_attributes) + ".pdf"
        )
        plt.savefig(filename, bbox_inches="tight")
        print("File saved: " + filename)
        plt.close(ax.get_figure())


def plot_results(df, subplots=False, print_df=False):
    if print_df:
        print(df)

    if subplots:
        sub_plot_durations(df)
        sub_plot_memory(df)
    else:  # subplots == False
        index_keys = df.index.unique()
        color_mapping = competitor_color_mapping()
        figsize = (4, 4)
        for index_key in index_keys:
            case, distribution, ordering = index_key
            sub_df = df.loc[index_key]

            # Filter competitors
            # if case == "Insert":
            #     sub_df = sub_df.loc[sub_df[c_index_name].isin(["BB-Tree", "Skip List", "B+ Tree TLX"])]
            # elif case == "EqualityLookup":
            #     sub_df = sub_df.loc[sub_df[c_index_name].isin(["BB-Tree", "Skip List", "Sorted Vector"])]

            # print(sub_df.to_string())
            plot_df = sub_df.pivot(index=c_data_size, columns=c_index_name, values=[c_duration, c_index_size])
            # print(plot_df)
            style = generate_style_for_competitors(plot_df.columns.get_level_values(1).unique())
            ax = plot_df.plot.line(
                figsize=figsize,
                y=c_duration,
                legend=True,
                color=color_mapping,
                style=style,
                fillstyle="none",
                markersize=MARKER_SIZE,
            )
            ax.get_xaxis().set_major_formatter(tkr.FuncFormatter(lambda x, p: "{0:.0f}".format(x * 10 ** (-6))))
            ax.get_xaxis().set_major_locator(tkr.IndexLocator(base=2000000, offset=0))
            # ax.get_xaxis().set_major_locator(tkr.MaxNLocator(min_n_ticks=5))
            ax.set_xlabel("Number of index entries in million")

            ax.get_yaxis().set_major_formatter(tkr.FuncFormatter(lambda y, p: "{0:.0f}".format(y * 10 ** (-6))))
            ax.set_ylabel("Duration in milliseconds")

            if case == "Insert":
                ax.get_yaxis().set_major_formatter(tkr.FuncFormatter(lambda y, p: "{0:.1f}".format(y * 10 ** (-9))))
                ax.set_ylabel("Duration in seconds")

            ax.legend(
                title="",
                bbox_to_anchor=(0.0, 1.02, 1.0, 0.102),
                loc="lower left",
                ncol=3,
                mode="expand",
                borderaxespad=0.0,
            )
            # ax.legend(title="", bbox_to_anchor=(1.02, 1), loc='upper left', borderaxespad=0.)
            plt.tight_layout()
            data_description_attributes = [distribution, ordering]
            data_description_attributes.extend(sub_df[[c_key_type, c_val_type]].iloc[0].values)
            filename = "../" + case + "_duration_" + "_".join(data_description_attributes) + ".pdf"
            plt.savefig(filename)
            print("File saved: " + filename)
            plt.close(ax.get_figure())
            # memory footprint
            if not plot_df[c_index_size].isnull().values.all():
                ax = plot_df.plot.line(
                    figsize=figsize,
                    y=c_index_size,
                    legend=True,
                    color=color_mapping,
                    style=style,
                    fillstyle="none",
                    markersize=4,
                )
                ax.set_xlabel("Number of index entries in million")
                ax.set_ylabel("Size in bytes")
                ax.legend(
                    title="",
                    bbox_to_anchor=(0.0, 1.02, 1.0, 0.102),
                    loc="lower left",
                    ncol=3,
                    mode="expand",
                    borderaxespad=0.0,
                )
                # ax.legend(title="", bbox_to_anchor=(1.02, 1), loc='upper left', borderaxespad=0.)
                plt.tight_layout()
                filename = "../" + case + "_index_size_" + "_".join(data_description_attributes) + ".pdf"
                plt.savefig(filename)
                print("File saved: " + filename)
                plt.close(ax.get_figure())


if __name__ == "__main__":
    subplots = False
    if len(sys.argv) == 2:
        if sys.argv[1] == "subplots":
            subplots = True
        else:
            exit("Unknown option: " + sys.argv[1])
    df = create_df()
    plot_results(df, subplots=subplots, print_df=False)
