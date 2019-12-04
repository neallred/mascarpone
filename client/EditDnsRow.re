let css = ReactDOMRe.Style.make;
let cx = ReactDOMRe.Style.combine;
let str = ReasonReact.string;
let _0 = ReasonReact.null;
let tgt = ReactEvent.Form.target;

let rowClass = css(
  ~position="relative",
  ~padding="5px 0",
  ()
);
let cellClass = css(
  ~width="33%",
  ~display="inline-block",
  ()
);

type editItem = {
  domain: string,
  newDomain: string,
  redirect: string,
};

type action =
  | UpdateDomain(string)
  | UpdateRedirect(string);

let reducer = (state, action) => {
  switch (action) {
  | UpdateDomain(newDomain) => {...state, newDomain}
  | UpdateRedirect(redirect) => {...state, redirect}
  }
};

[@react.component]
let make = (
  ~dnsRecord: DnsRow.dnsRecord,
  ~cancelEdit,
  ~confirmEdit,
) => {
  let (state, dispatch) = React.useReducer(reducer, {
    domain: dnsRecord.domain,
    newDomain: dnsRecord.domain,
    redirect: dnsRecord.redirect,
  } );
  <div className={"dns-row"} style={rowClass}>
    <input
      style={cx(Styles.baseInput, cellClass)}
      value={state.newDomain}
      onChange={(e) => {
        dispatch(UpdateDomain(e->tgt##value))
      }}
    />
    <input
      style={cx(Styles.baseInput, cellClass)}
      value={state.redirect}
      onChange={(e) => {
        dispatch(UpdateRedirect(e->tgt##value))
      }}
    />
    <RowControls
      no={_ => cancelEdit()}
      noWord="Cancel"
      yes={_ => confirmEdit(state)}
      yesWord="Confirm"
    />

    <div style={cellClass}>{str(Utils.format_date(dnsRecord.updated))}</div>
  </div>
};
