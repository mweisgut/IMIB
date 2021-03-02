#!/usr/bin/env python3

from plot_multi_json_results import create_df
from plot_multi_json_results import competitor_color_mapping
from plot_multi_json_results import generate_style_for_competitors
import matplotlib.pyplot as plt
import matplotlib.ticker as tck
import pandas as pd

COL_CASE_NAME = "cases.case_name"
COL_DATA_DESC = "data_description"
COL_DATA_DISTR = "data_distribution"
COL_DATA_ORDER = "data_ordering"
COL_DATA_SIZE = "cases.data_size"
COL_DURATION = "cases.executions.duration_ns"
COL_INDEX_NAME = "cases.index_name"
COL_INDEX_SIZE = "cases.executions.index_size_bytes"
COL_KEY_TYPE = "cases.key_type"
COL_VAL_TYPE = "cases.value_type"

PLOT_FIGSIZE = (6, 4)

if __name__ == "__main__":
    df = create_df()
    comparison_cases = ["BulkInsert", "BulkLoad"]
    include_competitors = ["TLX B+ Tree", "Abseil B-Tree", "TSL Robin Map"]
    cases = df.index.get_level_values(0).unique()
    assert set(comparison_cases).issubset(set(cases))
    distributions = df.index.get_level_values(1).unique()
    orderings = df.index.get_level_values(2).unique()
    print(cases, distributions, orderings)
    target_data_size = 10000000
    df = df[df[COL_DATA_SIZE] == target_data_size]
    # print(df[COL_INDEX_NAME].unique())
    df = df[df[COL_INDEX_NAME].isin(include_competitors)]
    df = df.drop(columns=[COL_INDEX_SIZE, COL_KEY_TYPE, COL_VAL_TYPE, COL_DATA_SIZE])
    df = df.loc[(comparison_cases, slice(None), slice(None))]
    df.sort_index(inplace=True)
    df.reset_index(inplace=True)
    df = df.pivot(index=[COL_CASE_NAME, COL_DATA_DISTR, COL_DATA_ORDER], columns=COL_INDEX_NAME, values=COL_DURATION)
    # df.dropna(inplace=True, axis="columns")
    # print(df.to_string())
    # df.reset_index(inplace=True)
    # print(df.to_string())
    case_1_df = df.loc[(comparison_cases[0], slice(None), slice(None))]
    case_2_df = df.loc[(comparison_cases[1], slice(None), slice(None))]
    case_1_df = case_1_df.droplevel(COL_CASE_NAME)
    case_2_df = case_2_df.droplevel(COL_CASE_NAME)
    case_2_df = case_2_df.drop(columns=["Abseil B-Tree", "TSL Robin Map"])
    case_1_df = case_1_df.add_suffix(" (Bulk Insert)")
    case_2_df = case_2_df.add_suffix(" (Bulk Load)")
    plot_df = pd.concat([case_1_df, case_2_df], axis="columns")
    competitors = plot_df.columns
    plot_df.reset_index(inplace=True)
    plot_df[COL_DATA_ORDER].replace({"asc": "ascending", "shuffled": "random"}, inplace=True)
    plot_df[COL_DATA_DESC] = plot_df[COL_DATA_DISTR] + "\n" + plot_df[COL_DATA_ORDER]
    plot_df = plot_df.drop(columns=[COL_DATA_DISTR, COL_DATA_ORDER])
    print(plot_df)

    fig = plt.figure(figsize=PLOT_FIGSIZE)
    ax = fig.add_subplot(111)
    # ax.axhline(y=0, color="black", linewidth=0.6)
    ax.yaxis.grid(linewidth=0.5)
    ax.set_xlabel("Data characteristics", labelpad=10)
    ax.set_ylabel("Execution time in seconds")
    ax.get_yaxis().set_major_formatter(tck.FuncFormatter(lambda y, p: "{0:.0f}".format(y * 10 ** (-9))))
    # ax.get_yaxis().set_major_locator(tck.IndexLocator(base=0.1, offset=0))

    comp_names = [c.rsplit(" ", 2)[0] for c in competitors]
    color_mapping = competitor_color_mapping()
    styles = generate_style_for_competitors(comp_names)
    for idx, competitor in enumerate(competitors):
        facecolors = "none"
        if competitor.endswith("(Bulk Insert)"):
            comp_marker = styles[idx][0]
        else:
            comp_marker = "X"

        if comp_marker == "+":
            facecolors = None

        comp_color = color_mapping[competitor.rsplit(" ", 2)[0]]
        ax.scatter(
            plot_df[COL_DATA_DESC],
            plot_df[competitor],
            s=40,
            color=comp_color,
            marker=comp_marker,
            label=competitor,
            facecolors=facecolors,
        )

    plt.legend(bbox_to_anchor=(0.0, 1.02, 1.0, 0.102), loc="lower left", ncol=2, mode="expand", borderaxespad=0.0)

    # plt.legend(loc="upper right")
    plt.tight_layout()
    data_description_attributes = comparison_cases
    filename = "../" + "Comp_" + "_".join(data_description_attributes) + ".pdf"
    # plt.tight_layout()
    plt.savefig(filename, bbox_inches="tight")
    print("File saved: " + filename)
    plt.close(ax.get_figure())
