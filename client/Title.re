let css = ReactDOMRe.Style.make;
let str = ReasonReact.string;

let countClass = css(
  ~fontSize=Styles.fontSizes.main,
  ~display="inline-block",
  ~width="100px",
  ()
);

let titleClass = css(
  ~fontSize="2em",
  ()
);

let title = str("Mascarpone internet filterer");

[@react.component]
let make = (
  ~visible,
  ~total,
) => {
  let countVerbiage = str(
    " (" ++
    string_of_int(visible) ++
    " of " ++
    string_of_int(total) ++
    ")"
  );

  <div>
    <span style={titleClass}>{title}</span>
    <span style={countClass}>{countVerbiage}</span>
  </div>
};
