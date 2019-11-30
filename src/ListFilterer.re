let css = ReactDOMRe.Style.make;
let cx = ReactDOMRe.Style.combine;
let str = ReasonReact.string;
let _0 = ReasonReact.null;
let tgt = ReactEvent.Form.target;

let inputClass = cx(Styles.baseInput, css(
  ~minWidth="250px",
  (),
));
let filterWrapperClass = css(
  ~display="inline-block",
  ~position="relative",
  (),
);
let clearSearchClass = css(
  ~position="absolute",
  ~top="0",
  ~right="-22px",
  ~bottom="0",
  ~width="22px",
  (),
);

[@react.component]
let make = (
  ~searchTerm,
  ~isAddingNew,
  ~updateSearchTerm,
) => {
  let isClearable = String.length(searchTerm) > 0;

  <div>
    <div
      style={filterWrapperClass}
    >
      <input
        placeholder="Search domains or IP addresses"
        value={searchTerm}
        disabled={isAddingNew}
        onChange={(e) => {
          updateSearchTerm(e->tgt##value)
        }}
        style={inputClass}
      />
      {isClearable ? <button
        disabled={isAddingNew}
        style={clearSearchClass}
        onClick={_ => { updateSearchTerm(""); }}
      >
        {str("x")}
      </button> : _0}
    </div>
  </div>
};
