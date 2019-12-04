let css = ReactDOMRe.Style.make;
let str = ReasonReact.string;
let tgt = ReactEvent.Mouse.target;

let buttonClass = css(
  ~backgroundColor=Styles.palette.success,
  ~position="fixed",
  ~appearance="none",
  ~bottom="15px",
  ~right="15px",
  ~width="60px",
  ~height="60px",
  ~borderRadius="100%",
  ~boxShadow=Styles.baseShadow,
  ~fontSize="3em",
  ~borderStyle="none",
  (),
);

let buttonTextClass = css(
  ~position="absolute",
  ~top="0px",
  ~right="0px",
  (),
);

[@react.component]
let make = (
  ~isOpen,
  ~openNewRecord,
) => {
  <button
    onClick={e => {
      e->tgt##blur();
      openNewRecord(e)
    }}
    disabled={isOpen}
    style={buttonClass}
  >
    <span>{str("+")}</span>
  </button>
};
