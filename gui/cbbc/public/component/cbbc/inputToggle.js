class InputToggle extends React.Component {
  static propTypes = {
    no: PropTypes.string,
    label: PropTypes.string,
    name: PropTypes.string,
    value: PropTypes.string,
    btnName1: PropTypes.string,
    btnName2: PropTypes.string,
    type: PropTypes.string,
    feedback: PropTypes.string,
    responseResult: PropTypes.string,
    placeholder: PropTypes.string,
    isDisabled: PropTypes.bool,
    isDisabledTv: PropTypes.bool,
    onChange: PropTypes.func,
  }
  
  componentDidMount() {
    var that = this
    
    ui.chkToggle = $('.chkToggle')
    ui.chkToggle.bootstrapToggle({
      size: 'small',
      on: that.props.btnName1,
      off: that.props.btnName2
    });
    
    var id = this.props.name+'_'+this.props.no
    $('#'+id).change(function() {
      var name = $(this).data('name'),
          no = parseInt($(this).data('no')),
          wtype = $(this).data('wtype'),
          states = that.props.getStates()
          
      // save
      var obj = $.extend(true, {}, states.cells[no].setting)
      if (obj[name].value == 0)
        obj[name].value = '1'
      else if (obj[name].value == 1)
        obj[name].value = '0'
      states.cells[no].setting = obj
      that.props.setStates({cells: states.cells})
      
      // send
      var obj2 = $.extend(true, {}, states)
      var code = obj2.cells[no].action.code.value
      var command1 = {
        cmd: "param",
        action: "param",
        warrant_code: parseInt(code),
        type: name.toUpperCase(),
        value: obj[name].value,
        ref: obj2.prefix+no,
        id: states.userId,
        algo_name: states.modules[wtype]
      }
      sendWebsocket(JSON.stringify(command1))
    })
  }
  
  render() {
    if (!this.props.feedback && !this.props.responseResult)
      var cssInput = ''
    else if (this.props.feedback)
      var cssInput = 'is-invalid'
    else if (this.props.responseResult)
      var cssInput = 'is-valid'
    
    // 没有code or 有错误数据
    var isBtnDisabled = (this.props.feedback || this.props.isDisabled) ? true : false
    var id = this.props.name+'_'+this.props.no

    if (this.props.value == 1)
      var isChecked = 'checked'
    else
      var isChecked = ''
    
    return(
      <div className="form-group row">
        <label className="col-5 col-sm-5 col-form-label col-form-label-sm">
          {this.props.label}
        </label>
        <div className="col-4 col-sm-4"> 
        </div>
        <div className="col-3 col-sm-3">
          <input 
            id={id}
            className="chkToggle"
            type="checkbox"
            data-toggle="toggle"
            data-onstyle="secondary"
            data-name={this.props.name}
            data-no={this.props.no}
            data-wtype={this.props.type}
            checked={isChecked}
            disabled={isBtnDisabled}
            onChange={this.props.onChange}
          />
        </div>
        <div className="col-5 col-sm-5"></div>
        <div className="col-7 col-sm-7">
          <div className="invalid-feedback d-block">
            {this.props.feedback}
          </div>
        </div>
      </div>
    )
  }
}