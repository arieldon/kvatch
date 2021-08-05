#!/usr/bin/env python3

import json
import subprocess
import urllib.request


OK = 200
CREATED = 201
NO_CONTENT = 204


def req(method: str = "GET", data: bytes = b"") -> urllib.request.Request:
    return urllib.request.Request(
        url="http://localhost:4000/some_key",
        method=method,
        data=data,
    )


def put() -> None:
    assert urllib.request.urlopen(
        req(method="PUT", data=b"some value")
    ).status == CREATED


def get() -> None:
    assert (r := urllib.request.urlopen(req())).status == OK
    assert json.loads(r.read().decode("utf-8"))["some_key"] == "some value"


def delete() -> None:
    assert urllib.request.urlopen(req(method="DELETE")).status == NO_CONTENT


def main() -> None:
    kvatch = subprocess.Popen("../kvatch")

    put()
    get()
    delete()

    print("complete")
    kvatch.terminate()


if __name__ == "__main__":
    main()
