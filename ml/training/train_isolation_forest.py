from pathlib import Path
import argparse
import sqlite3

import joblib
import pandas as pd
from sklearn.compose import ColumnTransformer
from sklearn.ensemble import IsolationForest
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import OneHotEncoder, StandardScaler


FEATURES = [
    "duration",
    "packets_per_second",
    "bytes_per_second",
    "protocol",
    "connection_count",
    "failed_request_count",
    "traffic_frequency",
]


def load_flows(database_path: Path) -> pd.DataFrame:
    with sqlite3.connect(database_path) as connection:
        flows = pd.read_sql_query(
            """
            SELECT source_ip, protocol, packet_count, bytes_transferred, duration
            FROM TrafficFlow
            """,
            connection,
        )

    if flows.empty:
        raise ValueError("TrafficFlow table is empty. Seed data or run packet ingestion first.")

    duration = flows["duration"].clip(lower=0.001)
    flows["packets_per_second"] = flows["packet_count"] / duration
    flows["bytes_per_second"] = flows["bytes_transferred"] / duration
    flows["connection_count"] = flows.groupby("source_ip")["source_ip"].transform("count")
    flows["failed_request_count"] = 0
    flows["traffic_frequency"] = flows.groupby("source_ip")["packet_count"].transform("sum")
    return flows[FEATURES]


def train(database_path: Path, model_path: Path) -> None:
    data = load_flows(database_path)
    pipeline = Pipeline(
        steps=[
            (
                "features",
                ColumnTransformer(
                    transformers=[
                        (
                            "numeric",
                            StandardScaler(),
                            [
                                "duration",
                                "packets_per_second",
                                "bytes_per_second",
                                "connection_count",
                                "failed_request_count",
                                "traffic_frequency",
                            ],
                        ),
                        ("protocol", OneHotEncoder(handle_unknown="ignore"), ["protocol"]),
                    ]
                ),
            ),
            ("model", IsolationForest(contamination=0.08, random_state=42)),
        ]
    )
    pipeline.fit(data)
    model_path.parent.mkdir(parents=True, exist_ok=True)
    joblib.dump(pipeline, model_path)
    print(f"Saved Isolation Forest model to {model_path}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--database", default="../../database/nids.sqlite")
    parser.add_argument("--model", default="../models/isolation_forest.joblib")
    args = parser.parse_args()

    train(Path(args.database).resolve(), Path(args.model).resolve())

