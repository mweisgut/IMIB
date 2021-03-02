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
    comparison_cases = ["Insert", "BulkInsert", "BulkLoad"]
    exclude_competitors = ["Simple Vector", "Sorted Vector"]
    cases = df.index.get_level_values(0).unique()
    assert set(comparison_cases).issubset(set(cases))
    distributions = df.index.get_level_values(1).unique()
    orderings = df.index.get_level_values(2).unique()
    print(cases, distributions, orderings)
    target_data_size = 10000000
    df = df[df[COL_DATA_SIZE] == target_data_size]
    # print(df[COL_INDEX_NAME].unique())
    df = df[~df[COL_INDEX_NAME].isin(exclude_competitors)]
    assert not set(exclude_competitors).issubset(set(df[COL_INDEX_NAME].unique()))
    df = df.drop(columns=[COL_INDEX_SIZE, COL_KEY_TYPE, COL_VAL_TYPE, COL_DATA_SIZE])
    df = df.loc[(comparison_cases, slice(None), slice(None))]
    df.sort_index(inplace=True)
    df.reset_index(inplace=True)
    df = df.pivot(index=[COL_CASE_NAME, COL_DATA_DISTR, COL_DATA_ORDER], columns=COL_INDEX_NAME, values=COL_DURATION)
    # df.dropna(inplace=True, axis="columns")
    # df.reset_index(inplace=True)
    # print(df.to_string())
    insert_df = df.loc[("Insert", slice(None), slice(None))]
    binsert_df = df.loc[("BulkInsert", slice(None), slice(None))]
    bload_df = df.loc[("BulkLoad", slice(None), slice(None))]
    insert_df = insert_df.droplevel(COL_CASE_NAME)
    binsert_df = binsert_df.droplevel(COL_CASE_NAME)
    bload_df = bload_df.droplevel(COL_CASE_NAME)
    bload_df = bload_df["TLX B+ Tree"].to_frame()
    # rel_change_df = binsert_df.sub(insert_df).div(insert_df)
    rel_bi_change_df = binsert_df.div(insert_df).sub(1)
    rel_bl_change_df = bload_df.div(insert_df).sub(1)
    rel_bl_change_df.dropna(axis="columns", how="all", inplace=True)
    rel_bi_change_df = rel_bi_change_df.add_suffix(" (Bulk Insert)")
    rel_bl_change_df = rel_bl_change_df.add_suffix(" (Bulk Load)")
    rel_change_df = pd.concat([rel_bi_change_df, rel_bl_change_df], axis=1)

    rel_change_df = rel_change_df.loc[:, abs(rel_change_df).ge(0.02).any()]
    print(rel_change_df)
    rel_change_df = rel_change_df.T
    rel_change_df = rel_change_df.unstack().to_frame()
    rel_change_df.reset_index(inplace=True)
    rel_change_df[COL_DATA_ORDER].replace({"asc": "ascending", "shuffled": "random"}, inplace=True)
    rel_change_df[COL_DATA_DESC] = rel_change_df[COL_DATA_DISTR] + "\n" + rel_change_df[COL_DATA_ORDER]
    rel_change_df = rel_change_df.drop(columns=[COL_DATA_DISTR, COL_DATA_ORDER])
    rel_change_df.rename(columns={0: "rel_duration_change", "level_2": COL_INDEX_NAME}, inplace=True)
    rel_change_df.dropna(inplace=True, axis="index")
    rel_change_df.set_index(COL_INDEX_NAME, inplace=True)
    # rel_change_df.sort_index(inplace=True)

    fig = plt.figure(figsize=PLOT_FIGSIZE)
    ax = fig.add_subplot(111)
    ax.axhline(y=0, color="black", linewidth=0.6)
    ax.yaxis.grid(linewidth=0.5)
    ax.set_xlabel("Data characteristics", labelpad=10)
    ax.set_ylabel("Relative execution time change")
    ax.get_yaxis().set_major_formatter(tck.FuncFormatter(lambda y, p: "{0:.0f}%".format(y * 100)))
    # ax.get_yaxis().set_major_locator(tck.IndexLocator(base=0.1, offset=0))

    competitors_with_suffix = rel_change_df.index.unique()
    competitors = [x.rsplit(" ", 2)[0] for x in competitors_with_suffix]
    color_mapping = competitor_color_mapping()
    styles = generate_style_for_competitors(competitors)
    for idx, comp_with_suffix in enumerate(competitors_with_suffix):
        sub_df = rel_change_df.loc[comp_with_suffix]
        comp_marker = styles[idx][0]
        comp_color = color_mapping[competitors[idx]]
        facecolors = "none"
        if comp_marker == "+":
            facecolors = None
        if comp_with_suffix == "TLX B+ Tree (Bulk Load)":
            comp_marker = "X"

        ax.scatter(
            sub_df[COL_DATA_DESC],
            sub_df["rel_duration_change"],
            s=40,
            color=comp_color,
            marker=comp_marker,
            label=comp_with_suffix,
            facecolors=facecolors,
        )

    plt.legend(bbox_to_anchor=(0.0, 1.02, 1.0, 0.102), loc="lower left", ncol=2, mode="expand", borderaxespad=0.0)

    # plt.legend(loc="upper right")
    plt.tight_layout()
    data_description_attributes = comparison_cases
    filename = "../" + "Relative_execution_time_change_" + "_".join(data_description_attributes) + ".pdf"
    # plt.tight_layout()
    plt.savefig(filename, bbox_inches="tight")
    print("File saved: " + filename)
    plt.close(ax.get_figure())
