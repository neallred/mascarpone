let css = ReactDOMRe.Style.make;
let cx = ReactDOMRe.Style.combine;
let str = ReasonReact.string;

let startDeleteClass = css(
  ~color="#ff3232",
  ()
);

let rowControlButtonClass = color => cx(Styles.baseButton, css(
  ~marginLeft="3px",
  ~backgroundColor=color,
  ~boxShadow=Styles.baseShadow,
  ()
));

let rowControlsClass = css(
  ~display="none",
  ~position="absolute",
  ~alignItems="center",
  ~justifyContent="space-around",
  ~top="0px",
  ~right="3px",
  ~zIndex="0",
  ()
);


[@react.component]
let make = (
  ~no,
  ~noWord,
  ~noColor=Styles.palette.info,
  ~yes,
  ~yesWord,
  ~yesColor=Styles.palette.success,
) => <div className={"dns-row__controls"} style={rowControlsClass}>
  <button style={rowControlButtonClass(noColor)} onClick={no}>{str(noWord)}</button>
  <button style={rowControlButtonClass(yesColor)} onClick={yes}>{str(yesWord)}</button>
</div>;
