let css = ReactDOMRe.Style.make;
let cx = ReactDOMRe.Style.combine;
let str = ReasonReact.string;
let _0 = ReasonReact.null;
let tgt = ReactEvent.Form.target;

let buttonClass = backgroundColor => cx(Styles.baseButton, css(
  ~backgroundColor=backgroundColor,
  ~boxShadow=Styles.baseShadow,
  ()
));

let formActionClass = css(
  ~display="flex",
  ~justifyContent="space-around",
  ~alignItems="center",
  ()
);

let validationClass = css(
  ~paddingBottom="10px",
  ~color=Styles.palette.fail,
  (),
)

let newRecordWrapperClass = css(
  ~overflowX="hidden",
  ~zIndex="1",
  (),
);

let newRecordWrapperOvertakeClass = css(
  ~zIndex="1",
  ~overflowX="hidden",
  ~top="0",
  ~right="0",
  ~bottom="0",
  ~left="0",
  ~position="fixed",
  (),
);

let slidingNewFormClass = (isOpen) => css(
  ~display="block",
  ~maxWidth="400px",
  ~width="100vw",
  ~height="70vh",
  ~maxHeight="700px",
  ~padding="10px",
  ~backgroundColor=Styles.palette.base,
  ~margin="15vh auto",
  ~opacity=(isOpen ? "1" : "0"),
  ~transform=(isOpen ? "translateX(0)" : "translateX(100vw)"),
  ~transition="max-width 0.2s ease, transform 0.2s ease, opacity 0.2s ease",
  ()
);

let darkenerClass = css(
  ~backgroundColor="rgba(0,0,0,0.4)",
  ~position="fixed",
  ~top="0px",
  ~right="0px",
  ~bottom="0px",
  ~left="0px",
  (),
)

type newItem = {
  domain: string,
  redirect: string,
};

let blankItem = {
  domain: "",
  redirect: AppConstants.localhost,
};

type action =
  | UpdateDomain(string)
  | UpdateRedirect(string);

let reducer = (state, action) => {
  switch (action) {
  | UpdateDomain(domain) => {...state, domain}
  | UpdateRedirect(redirect) => {...state, redirect}
  }
};

// let regexDomainNoProtocol = Js.Re.fromString({|^(((?!-))(xn--|_{1,1})?[a-z0-9-]{0,61}[a-z0-9]{1,1}\.)*(xn--)?([a-z0-9][a-z0-9\-]{0,60}|[a-z0-9-]{1,30}\.[a-z]{2,})$|});
let regexDomain = Js.Re.fromString({|^(https?://)?(((?!-))(xn--|_{1,1})?[a-z0-9-]{0,61}[a-z0-9]{1,1}\.)*(xn--)?([a-z0-9][a-z0-9\-]{0,60}|[a-z0-9-]{1,30}\.[a-z]{2,})$|});
let regexTld = Js.Re.fromString({|^(https?://)?[A-Za-z]+[A-Za-z0-9-]*$|});
let regexProtocol = Js.Re.fromString({|^(https?://)|});
let regexIp = Js.Re.fromString({|((^\s*((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))\s*$)|(^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$))|});

let validateDomain = (domain) => {
  Js.Re.test_(regexDomain, domain)
};

let isTld = (domain) => {
  Js.Re.test_(regexTld, domain)
};

let validateIp = (ip) => {
  Js.Re.test_(regexIp, ip)
};


let time_to_finish_animation = 300;
[@react.component]
let make = (
  ~isOpen,
  ~closeAddNew,
  ~confirmAddNew,
) => {
  let (state, dispatch) = React.useReducer(reducer, blankItem);
  let firstInputRef = React.useRef(Js.Nullable.null);

  React.useEffect1(() => {
    let current = React.Ref.current(firstInputRef);
    if (isOpen) {
      switch(Js.Nullable.toOption(current)) {
        | Some(element) => {
          let elementObj = ReactDOMRe.domElementToObj(element);
          let _ = Js.Global.setTimeout(() => {
            elementObj##focus();
          }, time_to_finish_animation);
          ()
        }
        | None => {
          Js.log("Wanted to focus first input in modal, but did not happen");
        }
      }
    }
    None
  }, [|isOpen|]);

  let {
    domain,
    redirect,
  } = state;

  if (!isOpen) {
    if (domain != blankItem.domain) {
      dispatch(UpdateDomain(blankItem.domain));
    }
    if (redirect != blankItem.redirect) {
      dispatch(UpdateRedirect(blankItem.redirect));
    }
  };

  let domainValid = validateDomain(domain);
  let domainIsTld = isTld(domain);
  let ipValid = validateIp(redirect);
  let canSubmit = domainValid && !domainIsTld && ipValid && isOpen;
  let sansProtocol = Js.String.replaceByRe(regexProtocol, "", domain);

  let message:string = if (domainValid) {
    if (domainIsTld) {
      {j|"top level domains not allowed. Entering "$sansProtocol" would block every site ending in "$sansProtocol"; for example mysite. $sansProtocol|j}
    } else {
      ""
    }
  } else {
    "Please enter a valid web site domain."
  };


  <div style={isOpen ? newRecordWrapperOvertakeClass : newRecordWrapperClass}>
    { isOpen ? <div style={darkenerClass}></div> : _0 }
    <form style={slidingNewFormClass(isOpen)}>
      <input
        disabled={!isOpen}
        placeholder="New domain"
        value={domain}
        onChange={(e) => {
          dispatch(UpdateDomain(e->tgt##value))
        }}
        style={Styles.baseInput}
        ref={ReactDOMRe.Ref.domRef(firstInputRef)}
      />
      <div style={validationClass}>
        {str(message)}
      </div>
      <input
        disabled={!isOpen}
        placeholder="New redirect"
        value={redirect}
        style={Styles.baseInput}
        onChange={(e) => {
          dispatch(UpdateRedirect(e->tgt##value))
        }}
      />
      <div style={validationClass}>
        {str(ipValid ? "" : "Please enter a valid ip address (e.g 127.0.0.1 for localhost)" )}
      </div>
      <div style={formActionClass}>
        <button
          onClick={e => {
            ReactEvent.Mouse.preventDefault(e);
            closeAddNew(e);
          }}
          disabled={!isOpen}
          style={buttonClass(Styles.palette.info)}
        >{str("Cancel")}</button>
        <button
          disabled={!canSubmit}
          style={buttonClass(canSubmit ? Styles.palette.success : Styles.palette.base)}
          onClick={e => {
            ReactEvent.Mouse.preventDefault(e);
            confirmAddNew(state);
          }}
        >
          {str("Confirm")}
        </button>
      </div>
    </form>
  </div>
};
