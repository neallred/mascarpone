let css = ReactDOMRe.Style.make;
let str = ReasonReact.string;
let _0 = ReasonReact.null;

let rowClass = isDeleting => css(
  ~position="relative",
  ~padding="5px 0",
  ~background=isDeleting ? Styles.palette.warn : "initial",
  ()
);
let cellClass = css(
  ~width="33%",
  ~display="inline-block",
  ()
);

type dnsRecord = {
  domain: string,
  redirect: string,
  updated: float,
};

let decodeDnsRecord= json =>
  Json.Decode.{
    domain: json |> field("domain", string),
    redirect: json |> field("redirect", string),
    updated: json |> field("updated", float),
  };

type partialDnsRecord = {
  redirect: string,
  updated: float,
}

let decodePartialDnsRecord = json =>
  Json.Decode.{
    redirect: json |> field("redirect", string),
    updated: json |> field("updated", float),
  };

[@react.component]
let make = (
  ~dnsRecord,
  ~canShowControls,
  ~requestDelete,
  ~requestEdit,
  ~isDeleting,
  ~confirmDelete,
  ~cancelDelete,
) => {
  let {
    domain,
    redirect,
    updated,
  } = dnsRecord;

  <div className={"dns-row"} style={rowClass(isDeleting)}>
    <div style={cellClass}>{str(domain)}</div>
    <div style={cellClass}>{str(redirect)}</div>

    <div style={cellClass}>{str(Utils.format_date(updated))}</div>
    {
      if (canShowControls) {
        <RowControls
          no={_ => requestDelete(domain)}
          noWord={"Delete"}
          noColor={Styles.palette.fail}
          yes={_ => requestEdit(domain)}
          yesWord={"Edit"}
        />
      } else if (isDeleting) {
        <RowControls
          no={_ => cancelDelete()}
          noWord={"Cancel"}
          yes={_ => confirmDelete(domain)}
          yesWord={"Delete"}
          yesColor={Styles.palette.fail}
        />
      } else {
        _0
      }
    }
  </div>
};
